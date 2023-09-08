#include "simple-shell.h"
#include "history.h"
#include <stdio.h>
#include <time.h>

struct history history_entries[100]; // Define the array here
int history_counter = 0; // Define the count here

void print_history() {
    // Loop through the history_entries array and print the command for each entry
    for (int i = 0; i < history_counter; i++) {
        printf("%s\n", history_entries[i].command);
    }
}

void SIGINT_history() {
    // Print all the history when the user presses ctrl-c
    for (int i = 0; i < history_counter; i++) {
        struct history entry = history_entries[i];

        printf("Command: %s\n", entry.command);
        printf("PID: %d\n", entry.entries[0]);
        printf("Start Time: %s", ctime((const time_t *)&entry.entries[1])); // Convert time to a readable format
        if (entry.entries[2] != -1) {
            printf("Execution Time: %d seconds\n", entry.entries[2]);
        } else {
            printf("Execution Time: N/A\n");
        }
        printf("\n");
    }
}
