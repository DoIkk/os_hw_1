#include "shell.h"

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