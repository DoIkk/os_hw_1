#include "shell.h"

int main(void){
    char cmd[MAX];

    while(1){
        printPrompt();
        fgets(cmd, sizeof(cmd) - 1, stdin);

        if (execute(cmd) == false) {
            break;
        }
    }
    return 0;
}
