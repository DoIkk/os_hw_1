#include "shell.h"

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

int tokenize(char *cmd, char *argv[100], char **input_file, char **output_file, int *append) {
    int tokensize = 0;
    char *saveptr;
    *input_file = NULL;    // 입력 파일을 초기화
    *output_file = NULL;   // 출력 파일을 초기화
    *append = 0;           // 덮어쓰기로 설정

    argv[tokensize] = strtok_r(cmd, DELIMS, &saveptr);  // 첫 번째 토큰 추출
    while (argv[tokensize] != NULL) {
        if (strcmp(argv[tokensize], ">") == 0) {
            // 출력 리다이렉션 (덮어쓰기 모드)
            argv[tokensize] = NULL;  // ">" 기호를 명령어 배열에서 제거
            *output_file = strtok_r(NULL, DELIMS, &saveptr);  // 출력할 파일 이름을 가져옴
        } else if (strcmp(argv[tokensize], ">>") == 0) {
            // 출력 리다이렉션 (추가 모드)
            argv[tokensize] = NULL;  // ">>" 기호를 명령어 배열에서 제거
            *output_file = strtok_r(NULL, DELIMS, &saveptr);  // 출력할 파일 이름을 가져옴
            *append = 1;  // 추가 모드로 설정
        } else if (strcmp(argv[tokensize], "<") == 0) {
            // 입력 리다이렉션
            argv[tokensize] = NULL;  // "<" 기호를 명령어 배열에서 제거
            *input_file = strtok_r(NULL, DELIMS, &saveptr);  // 입력할 파일 이름을 가져옴
        } else {
            tokensize++;  // 명령어 배열에 토큰 추가
            argv[tokensize] = strtok_r(NULL, DELIMS, &saveptr);  // 다음 토큰을 추출
        }
    }
    argv[tokensize] = NULL;  // 명령어 배열 마지막에 NULL 추가
    return tokensize;  // 토큰 개수 반환
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
    return true;
}

// 도움말 출력 함수
void printHelp(void){
    printf("==========================================================\n");
    printf("쉘에서 지원하는 명령어 및 기능\n");

    printf("대부분의 기본 명령어 사용가능하며 외부명령어도 지원\n");
    printf("cd\t: Change the shell working directory.\n");
    printf("quit\t: Exit the shell.\n");
    printf("help\t: Display information about builtin commands.\n");
    printf("type\t: Display information about command type.\n");
    printf("pwd\t: Print the name of the current working directory.\n");
    printf("echo\t: Write arguments to the standard output.\n");

    printf("redirection 지원\n");
    printf("> [파일]\t\t: 명령어 출력을 해당 파일에 저장합니다 (덮어쓰기).\n");
    printf(">> [파일]\t\t: 명령어 출력을 해당 파일에 추가합니다.\n");
    printf("< [파일]\t\t: 파일로부터 명령어 입력을 받습니다.\n");
    printf("==========================================================\n");
}

void printPrompt(void){
    char *pwd = getcwd(NULL,0);    // 현재 작업 디렉토리
    char *user = getenv("USER");  // 사용자 이름
    time_t now = time(NULL);
    struct tm *local = localtime(&now);
    char time_str[9];
    strftime(time_str, sizeof(time_str), "%H:%M:%S", local);  // 현재 시간

    // 사용자 이름, 현재 디렉토리, 시간으로 프롬프트 표시
    if (user && pwd) {
        printf("%s [%s] %s$ ", pwd, time_str,user);  
    }     // 오류가 발생한 경우 적절한 오류 메시지를 출력
    else {
        if (!user) {
            fprintf(stderr, "Failed to retrieve user from environment.\n");
        }
        if (!pwd) {
            fprintf(stderr, "Failed to retrieve current directory.\n");
        }
        printf("$ ");  // 오류가 발생해도 기본 프롬프트 출력
    }
    free(pwd);
}