#include "shell.h"

// 외부 명령어 목록 배열 정의
char *extern_cmd_arr[] = {"quit", "cd", "help", "pwd", "type"};

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
            execvp(pathname, argv);  // argv 배열 전체를 전달하여 명령어 실행
            perror("execvp failed");  // 오류 발생 시
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
        char *curr_dir = getcwd(NULL, 0);
        printf("%s\n", curr_dir);
        free(curr_dir);
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

    // 명령어가 존재하지 않는 경우
    printf("%s: command not found\n", argv[0]);
    return true;
}