#include "shell.h"

// 외부 명령어 목록 배열 정의
char *extern_cmd_arr[] = {"quit", "cd", "help", "pwd", "type","echo"};

// 환경 변수 배열을 extern으로 가져오기
extern char **environ;

// 리다이렉션 여부를 확인하고 리다이렉션 처리
bool redirection(char *argv[], int tokensize) {
    int fd;
    
    for (int i = 0; i < tokensize; i++) {
        // 출력 리다이렉션 처리 (">")
        if (strcmp(argv[i], ">") == 0 && argv[i + 1] != NULL) {
            fd = open(argv[i + 1], O_WRONLY | O_CREAT | O_TRUNC, 0644); // 파일을 쓰기 모드로 열기
            if (fd == -1) {
                perror("open");
                return false;
            }
            dup2(fd, STDOUT_FILENO);  // 표준 출력을 파일로 리다이렉트
            close(fd);

            argv[i] = NULL;  // 리다이렉션 기호와 파일명을 argv에서 제거
            return true;
        }
        // 입력 리다이렉션 처리 ("<")
        if (strcmp(argv[i], "<") == 0 && argv[i + 1] != NULL) {
            fd = open(argv[i + 1], O_RDONLY);  // 파일을 읽기 모드로 열기
            if (fd == -1) {
                perror("open");
                return false;
            }
            dup2(fd, STDIN_FILENO);  // 표준 입력을 파일로 리다이렉트
            close(fd);

            argv[i] = NULL;  // 리다이렉션 기호와 파일명을 argv에서 제거
            return true;
        }
    }
    return false;
}

// execute 함수
bool execute(char *cmd) {
    char *argv[100];  // 명령어와 인자를 저장할 배열
    int tokensize = tokenize(cmd, argv);  // tokenize 함수로 명령어 분리
    char pathname[MAX];  // 명령어 경로를 저장할 변수

    // 아무것도 입력하지 않은 경우
    if (tokensize == 0) {
        return true;
    }

    // 외부 명령어가 PATH에 존재하는지 확인
    if (is_in_path(argv, pathname)) {
        // 프로세스 생성
        pid_t pid = fork();
        if (pid == -1) {  // fork 실패 시
            perror("fork error\n");
            return 0;
        } else if (pid == 0) {  // 자식 프로세스에서 명령어 실행
            execve(pathname,argv,environ);
            perror("execve failed");  // 오류 발생 시
            exit(EXIT_FAILURE);
        } else {  // 부모 프로세스는 자식이 종료될 때까지 기다림
            wait(NULL);
            return true;
        }
    }

    // 내부 명령어 처리
    if (strcmp(argv[0], "quit") == 0) {
        return false;
    }

    if (strcmp(argv[0], "cd") == 0) {
        char *home_dir = getenv("HOME");  // HOME 환경 변수 설정
        if (paramCheck(tokensize) == true) {
            int result = chdir(argv[1]);  // 경로 변경
            if (result == -1) {
                printf("%s: No such file or directory\n", argv[1]);
            }
        } else {
            chdir(home_dir);
        }
        return true;
    }

if (strcmp(argv[0], "pwd") == 0) {
    char *curr_dir = getcwd(NULL, 0);  // 현재 작업 디렉토리 가져오기
    {if (curr_dir != NULL) {
        printf("%s\n", curr_dir);  // 현재 작업 디렉토리 출력
        free(curr_dir);  // getcwd로 동적 할당된 메모리 해제
    } else {
        perror("getcwd error");  // 에러 발생 시 메시지 출력
    }}
    return true;
}

    if (strcmp(argv[0], "type") == 0) {
        for (int token = 1; argv[token] != NULL; token++) {
            char com_path[256];
            snprintf(com_path, sizeof(com_path), "/usr/bin/%s", argv[token]);
            if (access(com_path, F_OK) == 0) {
                printf("%s is an external command\n", argv[token]);
            } else if (is_in_arr(argv[token], extern_cmd_arr, 5)) {
                printf("%s is a shell builtin\n", argv[token]);
            } else {
                printf("%s: command not found\n", argv[token]);
            }
        }
        return true;
    }

    if (strcmp(argv[0], "help") == 0) {
        printHelp();
        return true;
    }

    if (strcmp(argv[0], "echo") == 0) {
        for (int i = 1; argv[i] != NULL; i++) {
            printf("%s ", argv[i]);
        }
        printf("\n");
        return true;
    }

    // 명령어가 존재하지 않는 경우
    printf("%s: command not found\n", argv[0]);
    return true;
}