#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <stdbool.h>

#define MAX 1024 //command의 max길이를 1024
#define DELIMS "\r\t\n " 

// 외부 명령어 목록
char *extern_cmd_arr[] = {"quit","cd","help","pwd","type"};

// PATH 환경 변수 참조
bool is_in_path(char *argv[100], char *final_path) {
    char *pathenv = getenv("PATH");  // 환경 변수 'PATH' 가져오기
    if (pathenv == NULL) {
        return false;  // PATH 환경 변수가 없는 경우
    }

    char *path_copy = strdup(pathenv);  // strtok 사용을 위한 복사본
    char *dir = strtok(path_copy, ":"); // ':' 구분자로 디렉토리 추출

    while (dir != NULL) {
        // 각 디렉토리에서 명령어의 경로 만들기
        snprintf(final_path, MAX, "%s/%s", dir, argv[0]);

        // 해당 경로에 실행 파일이 있는지 확인
        if (access(final_path, F_OK) == 0) {
            free(path_copy);  // 동적 메모리 해제
            return true;
        }

        // 다음 디렉토리로 이동
        dir = strtok(NULL, ":");
    }

    free(path_copy);  // 동적 메모리 해제
    return false;  // PATH에 없으면 false
}

int tokenize(char *cmd, char *argv[100]) {
    int tokensize = 0;
    char *saveptr;  // 상태 저장용 포인터

    // 첫 번째 토큰 추출
    argv[tokensize] = strtok_r(cmd, DELIMS, &saveptr);

    // 나머지 토큰 추출
    while (argv[tokensize] != NULL) {
        tokensize++;
        argv[tokensize] = strtok_r(NULL, DELIMS, &saveptr);
    }
    argv[tokensize] = NULL;  // 마지막에 NULL로 배열 종료 표시
    return tokensize;
}

// 배열 요소 검사
bool is_in_arr(char *element, char *arr[], int size) {
    for (int i = 0; i < size; i++) {
        if (strcmp(element, arr[i]) == 0) {
            return true;  // 요소가 리스트에 있음
        }
    }
    return false;  // 요소가 리스트에 없음
}

// 인자 체크
bool paramCheck(int tokensize){
    // 인자 없을경우 false return
    if(tokensize == 1){
        return false;
    }
    // 인자 있을경우 true를 반환
    else if(tokensize > 1){
        return true;
    } 
}

// 도움말 출력 함수
void printHelp(void){
    printf("==========================================================\n");
    printf("'>'과 '&'를 포함한 쉘에서 사용할 수 있는 명령어\n");
    printf("대부분의 shell 기본 명령어 사용가능\n");
    printf("cd\t: Change the shell working directory.\n");
    printf("quit\t: Exit the shell.\n");
    printf("help\t: Display information about builtin commands.\n");
    printf("type\t: Display information about command type.\n");
    printf("pwd\t: Print the name of the current working directory.\n");
    printf("==========================================================\n");
    return;

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

int main(void){
    char cmd[MAX];
    char * curr_dir;

    while(1){
        curr_dir = getcwd(NULL,0);
        printf("%s$ ",curr_dir);
        fgets(cmd,sizeof(cmd)-1,stdin);

        if (execute(cmd)==false){
            break;
        }
    }
    return 0;
}