#include "shell.h"

int main(void){
    char cmd[MAX];
    char *curr_dir;

    while(1){
        curr_dir = getcwd(NULL, 0);
        printf("%s$ ", curr_dir);
        fgets(cmd, sizeof(cmd) - 1, stdin);

        if (execute(cmd) == false) {
            break;
        }
    }
    return 0;
}
