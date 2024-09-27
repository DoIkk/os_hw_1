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
    return true;
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