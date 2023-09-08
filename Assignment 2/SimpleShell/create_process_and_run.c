#include "simple-shell.h"
#include "history.h"
#include <time.h>
#include <sys/time.h>
#include <string.h> // Add this for string manipulation functions

#define MAX_INPUT_SIZE 1024

int pipe_commands(char *command); // Function prototype
void print_history(); // Function prototype

int check_for_history(char *command) {
    // Check if the command is "history"
    if (strcmp(command, "history") == 0) {
        return 1;
    }

    return 0;
}

int check_for_cd(char *command) {
    // Check if the command starts with "cd "
    if (strncmp(command, "cd ", 3) == 0) {
        // Extract the path (substring after the space)
        return 1;
    }

    return 0;
}

int check_for_pipe(char *command) {
    // Check if the command contains a pipe character
    if (strchr(command, '|') != NULL) {
        return 1;
    }

    return 0;
}

int create_process_and_run(char *command) {
    if (check_for_history(command)) {
        // Handle the "history" command separately
        print_history();
        return 0; // Successful history command
    }

    if (check_for_cd(command)) {
        // Handle the "cd" command separately
        char *path = command + 3; // Skip "cd " in the command

        if (chdir(path) == -1) {
            perror("chdir");
            return 1;
        }

        // Record the "cd" command in the history array
        history_entries[history_counter].command = strdup(command);
        history_entries[history_counter].entries[0] = getpid(); // Current process ID
        history_entries[history_counter].entries[1] = time(NULL); // Current time
        history_entries[history_counter].entries[2] = -1; // Execution time not available
        history_counter++;

        return 0; // Successful cd command
    }

    if (check_for_pipe(command)) {
        // Handle piped commands
        return pipe_commands(command);
    }

    pid_t child_pid;
    int status;

    // Record the start time
    struct timeval start_time;
    gettimeofday(&start_time, NULL);

    // Fork a child process
    child_pid = fork();

    if (child_pid == -1) {
        perror("fork");
        return 1;
    } 
    
    else if (child_pid == 0) {
        // This code is executed by the child process
        // Execute the command using execvp
        if (execl("/bin/sh", "sh", "-c", command, NULL) == -1) {
            perror("execl");
            exit(EXIT_FAILURE);
        }

        exit(0);
    } 
    
    else {
        // This code is executed by the parent process
        // Wait for the child process to complete
        waitpid(child_pid, &status, 0);

        if (WIFEXITED(status)) {
            // Record the execution time duration
            struct timeval end_time;
            gettimeofday(&end_time, NULL);
            long long duration = (end_time.tv_sec - start_time.tv_sec) * 1000LL +
                                 (end_time.tv_usec - start_time.tv_usec) / 1000LL;

            // Update the history_entries array
            if (history_counter < 100) {
                history_entries[history_counter].command = strdup(command);
                history_entries[history_counter].entries[0] = child_pid;
                history_entries[history_counter].entries[1] = start_time.tv_sec;
                history_entries[history_counter].entries[2] = (int)duration;
                history_counter++;
            }

            return WEXITSTATUS(status);
        } 
        
        else {
            perror("waitpid");
            return 1;
        }
    }
}
