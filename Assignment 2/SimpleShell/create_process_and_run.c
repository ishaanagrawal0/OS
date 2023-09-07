#include "simple-shell.h"

int create_process_and_run(char *command) {
    pid_t pid, wpid;
    int status;

    // Tokenize the command into arguments
    char *args[256];  // Maximum number of arguments
    int arg_count = 0;
    char *token = strtok(command, " ");

    while (token != NULL) {
        args[arg_count++] = token;
        token = strtok(NULL, " ");
    }
    
    args[arg_count] = NULL;  // Null-terminate the argument list

    // Fork a new process
    pid = fork();

    if (pid == 0) {
        // Child process
        // Execute the command
        if (execvp(args[0], args) == -1) {
            perror("execvp");
            exit(EXIT_FAILURE);
        }
    } else if (pid < 0) {
        // Forking failed
        perror("fork");
        return -1;
    } else {
        // Parent process
        // Wait for the child to complete
        do {
            wpid = waitpid(pid, &status, WUNTRACED);
        } while (!WIFEXITED(status) && !WIFSIGNALED(status));
    }

    return 0;  // Return status code
}