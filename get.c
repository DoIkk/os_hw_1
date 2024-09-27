#include <stdio.h>
#include <stdlib.h>

int main() {
    char *path = getenv("PATH");  // "PATH"라는 이름의 환경 변수를 조회
    if (path != NULL) {
        printf("PATH: %s\n", path);  // "PATH" 환경 변수의 값을 출력
    } else {
        printf("PATH 환경 변수가 설정되어 있지 않습니다.\n");
    }

    return 0;
}
