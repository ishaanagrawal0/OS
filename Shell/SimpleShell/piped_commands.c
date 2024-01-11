#include "simple-shell.h"
#include "history.h"
#include <time.h>
#include <sys/time.h>

// For each command in the pipeline:
//   - Create pipes
//   - Fork a child process
//   - In the child process:
//     - Redirect stdin/stdout using dup2
//     - Execute the command using execv
//   - In the parent process, close unused pipe ends and wait for the child to complete.

#define MAX_INPUT_SIZE 1024

void pipe_commands(char *command) {
    // Split the command into separate commands based on |
    char *commands[MAX_INPUT_SIZE];
    int num_commands = 0;

    // Split the command into tokens and store them in the commands array
    char *token = strtok(command, "|");

    while (token != NULL && num_commands < MAX_INPUT_SIZE) {
        commands[num_commands] = token;
        num_commands++;
        token = strtok(NULL, "|");
    }

    // Null-terminate the commands array
    commands[num_commands] = NULL;

    int pipefd[2]; // File descriptors for the pipe
    pid_t child_pid;
    int status;

    int prev_pipe_read = STDIN_FILENO; // Initially, read from stdin

    // Concatenate the individual commands with the pipe symbol "|"
    char piped_command[MAX_INPUT_SIZE] = "";
    for (int i = 0; i < num_commands; i++) {
        strcat(piped_command, commands[i]);
        if (i < num_commands - 1) {
            strcat(piped_command, " | ");
        }
    }

    for (int i = 0; i < num_commands; i++) {
        // Create a pipe for each command except the last one
        if (i < num_commands - 1) {
            if (pipe(pipefd) == -1) {
                perror("pipe");
                exit(EXIT_FAILURE);
            }
        }

        // Fork a child process
        child_pid = fork();

        if (child_pid == -1) {
            perror("fork");
            exit(EXIT_FAILURE);
        } else if (child_pid == 0) {
            // This code is executed by the child process

            // Redirect input from the previous command's output
            if (i != 0) {
                if (dup2(prev_pipe_read, STDIN_FILENO) == -1) {
                    perror("dup2");
                    exit(EXIT_FAILURE);
                }
                close(prev_pipe_read); // Close the old pipe read end
            }

            // Redirect output to the next command's input
            if (i < num_commands - 1) {
                if (dup2(pipefd[1], STDOUT_FILENO) == -1) {
                    perror("dup2");
                    exit(EXIT_FAILURE);
                }
                close(pipefd[0]); // Close the pipe read end
            }

            // Close all pipe ends in the child process
            for (int j = 0; j < num_commands - 1; j++) {
                close(pipefd[j]);
            }

            // Split the current command into tokens
            char *args[MAX_INPUT_SIZE];
            int arg_count = 0;

            char *arg_token = strtok(commands[i], " ");
            while (arg_token != NULL && arg_count < MAX_INPUT_SIZE) {
                args[arg_count] = arg_token;
                arg_count++;
                arg_token = strtok(NULL, " ");
            }
            args[arg_count] = NULL;

            // Execute the command using execvp
            if (execvp(args[0], args) == -1) {
                perror("execvp");
                exit(EXIT_FAILURE);
            }
        } else {
            // This code is executed by the parent process

            // Close the write end of the current pipe (if it exists)
            if (i < num_commands - 1) {
                close(pipefd[1]);
            }

            // Close the read end of the previous pipe (if it exists)
            if (i != 0) {
                close(prev_pipe_read);
            }

            // Wait for the child process to complete
            waitpid(child_pid, &status, 0);

            if (WIFEXITED(status)) {
                // Record the concatenated piped command in the history array only once
                if (i == 0) {
                    history_entries[history_counter].command = strdup(piped_command);
                    history_entries[history_counter].entries[0] = child_pid;
                    history_entries[history_counter].entries[1] = time(NULL); // Current time
                    history_entries[history_counter].entries[2] = -1; // Execution time not available
                    history_counter++;
                }
            } else {
                perror("waitpid");
                exit(EXIT_FAILURE);
            }

            // Store the previous pipe read end for the next iteration
            prev_pipe_read = pipefd[0];
        }
    }
}
