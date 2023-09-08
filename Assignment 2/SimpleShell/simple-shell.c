#include "simple-shell.h"
#include "history.h"

char* read_user_input();
int create_process_and_run(char *command);
void SIGINT_history();

void my_handler(int signum) {
    if (signum == SIGINT) {
        SIGINT_history();
    }
}

int launch (char *command) {
    int status;
    status = create_process_and_run(command);
    return status;
}

int main() {
    struct sigaction sig;
    memset(&sig, 0, sizeof(sig));
    sig.sa_handler = my_handler;

    sigaction(SIGINT, &sig, NULL);

    do {
        printf("group_51@possum:~$ ");
        char* command = read_user_input();

        if (command != NULL) {
            int status = launch(command);
            free(command); // free the allocated memory for the command

            if (status != 0) {
                printf("Command exited with status %d\n", status);
            }
        }
    } while (1); // Loop indefinitely

    return 0;
}
