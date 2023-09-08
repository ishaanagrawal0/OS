#include "simple-shell.h"

#define MAX_INPUT_SIZE 1024

char* read_user_input() {
    char input[MAX_INPUT_SIZE];

    // Read user input
    if (fgets(input, sizeof(input), stdin) == NULL) {
        perror("fgets");
        exit(EXIT_FAILURE);
    }

    // Remove the newline character at the end, if present
    size_t input_length = strlen(input);
    if (input_length > 0 && input[input_length - 1] == '\n') {
        input[input_length - 1] = '\0';
    }

    // Check for backslashes or quotes in the input
    if (strchr(input, '\\') != NULL || strchr(input, '"') != NULL || strchr(input, '\'') != NULL) {
        printf("Error: Backslashes or quotes are not allowed in the command.\n");
        return NULL;
    }

    return strdup(input);
}