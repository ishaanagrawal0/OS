#include "simple-shell.h"

char* read_user_input();
int create_process_and_run(char *command);

int launch (char *command) {
    int status;
    status = create_process_and_run(command);
    return status;
}

int main() {
    while (1) {
        printf("group_51@possum:~$ ");
        char* command = read_user_input();

        if (command != NULL) {
            int status = launch(command);
            free(command); // free the allocated memory for the command

            if (status != 0) {
                printf("Command exited with status %d\n", status);
            }
        }
    }

    return 0;
}