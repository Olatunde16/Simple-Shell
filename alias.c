#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define MAX_ALIASES 64
#define MAX_ALIAS_NAME 64
#define MAX_ALIAS_VALUE 256

typedef struct {
    char name[MAX_ALIAS_NAME];
    char value[MAX_ALIAS_VALUE];
} Alias;

Alias aliases[MAX_ALIASES];
int num_aliases = 0;

/**
  * main - Checks if user entered "alias" command
  * print_aliases: Prints all aliases.
  * print_alias: Prints a specific alias by name.
  * define_alias: Defines or redefines an alias with a given name and value.
  *
  * Return: 0
  */

void print_aliases() {
    for (int i = 0; i < num_aliases; i++) {
        printf("%s='%s'\n", aliases[i].name, aliases[i].value);
    }
}

void print_alias(char *name) {
    for (int i = 0; i < num_aliases; i++) {
        if (strcmp(aliases[i].name, name) == 0) {
            printf("%s='%s'\n", aliases[i].name, aliases[i].value);
            return;
        }
    }
    printf("Alias '%s' not found.\n", name);
}

void define_alias(char *name, char *value) {
    // Check if the alias already exists
    for (int i = 0; i < num_aliases; i++) {
        if (strcmp(aliases[i].name, name) == 0) {
            strcpy(aliases[i].value, value);
            return;
        }
    }

    // If not, add a new alias
    if (num_aliases < MAX_ALIASES) {
        strcpy(aliases[num_aliases].name, name);
        strcpy(aliases[num_aliases].value, value);
        num_aliases++;
    } else {
        printf("Maximum number of aliases reached.\n");
    }
}

int main() {
    char input[MAX_INPUT_LENGTH];
    char *args[MAX_ARGS];
    int num_args;

    while (1) {
        // Display prompt
        printf("SimpleShell (PID: %d)> ", getpid());

        // Get user input
        fgets(input, MAX_INPUT_LENGTH, stdin);

        // Remove newline character
        input[strlen(input) - 1] = '\0';

        // Exit if user enters "exit"
        if (strcmp(input, "exit") == 0) {
            break;
        } else if (strcmp(input, "alias") == 0) {
            // Print all aliases
            print_aliases();
        } else if (strstr(input, "alias ") == input) {
            // Print specific alias
            char *alias_name = input + strlen("alias ");
            print_alias(alias_name);
        } else if (strstr(input, "alias ") != NULL && strstr(input, "='") != NULL) {
            // Define or redefine an alias
            char *alias_def = input + strlen("alias ");
            char *name = strtok(alias_def, "='");
            char *value = strtok(NULL, "'");
            define_alias(name, value);
        } else {
            // Tokenize the input into arguments
            char *token = strtok(input, " ");
            num_args = 0;

            while (token != NULL) {
                args[num_args++] = token;
                token = strtok(NULL, " ");
            }

            // Add a NULL pointer to the end of the args array (required by execvp)
            args[num_args] = NULL;

            // Check for logical operators
            int result = execute_command(args);

            // Handle && and ||
            if (result == 0 && strstr(input, "&&") != NULL) {
                // Execute the next command only if the previous one succeeded
                token = strtok(input, "&&");
                token = strtok(NULL, "&&");
                if (token != NULL) {
                    num_args = 0;
                    token = strtok(token, " ");
                    while (token != NULL) {
                        args[num_args++] = token;
                        token = strtok(NULL, " ");
                    }
                    args[num_args] = NULL;
                    result = execute_command(args);
                }
            } else if (result != 0 && strstr(input, "||") != NULL) {
                // Execute the next command only if the previous one failed
                token = strtok(input, "||");
                token = strtok(NULL, "||");
                if (token != NULL) {
                    num_args = 0;
                    token = strtok(token, " ");
                    while (token != NULL) {
                        args[num_args++] = token;
                        token = strtok(NULL, " ");
                    }
                    args[num_args] = NULL;
                    result = execute_command(args);
                }
            }
        }
    }

    return 0;
}
