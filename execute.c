#include "shell.h"

// 외부 명령어 목록 배열 정의
char *extern_cmd_arr[] = {"quit", "cd", "help", "pwd", "type","echo"};

// 환경 변수 배열을 extern으로 가져오기
extern char **environ;

// execute 함수
bool execute(char *cmd) {
    char *argv[100];
    char pathname[MAX];
    char *input_file = NULL;
    char *output_file = NULL;
    int append = 0;  // 0 for '>', 1 for '>>'
    
    int tokensize = tokenize(cmd, argv, &input_file, &output_file, &append);

    if (tokensize == 0) {
        return true;
    }

    // 외부 명령어
    if (is_in_path(argv, pathname)) {
        pid_t pid = fork();
        if (pid == -1) {
            perror("fork error\n");
            return false;
        } else if (pid == 0) {
            // Redirection
            if (input_file) {
                int fd_in = open(input_file, O_RDONLY);
                if (fd_in == -1) {
                    perror("input redirection error");
                    exit(EXIT_FAILURE);
                }
                dup2(fd_in, STDIN_FILENO);  // 표준 입력을 파일로 리다이렉트
                close(fd_in);
            }
            if (output_file) {
                int fd_out;
                if (append) {
                    fd_out = open(output_file, O_WRONLY | O_CREAT | O_APPEND, 0644);  // 추가
                } else {
                    fd_out = open(output_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);  // 덮어쓰기
                }
                if (fd_out == -1) {
                    perror("output redirection error");
                    exit(EXIT_FAILURE);
                }
                dup2(fd_out, STDOUT_FILENO);  // 표준 출력을 파일로 리다이렉트
                close(fd_out);
            }
            execve(pathname, argv, environ);
            perror("execve failed");
            exit(EXIT_FAILURE);
        } else {
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