#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <math.h>

void read_line(const char *prompt, char *buffer, int size) {
    while (1) {
        printf("%s", prompt);

        if (fgets(buffer, size, stdin) == NULL) {
            printf("\nInput ended\n");
            exit(EXIT_SUCCESS);
        }

        if (strchr(buffer, '\n') == NULL && !feof(stdin)) {
            int ch;
            while ((ch = getchar()) != '\n' && ch != EOF) {
                // Удаляем остаток слишком длинной строки.
            }

            printf("Input is too long\n");
            continue;
        }

        return;
    }
}

double read_number(const char *prompt) {
    char buffer[256];
    char *end;

    while (1) {
        read_line(prompt, buffer, sizeof(buffer));

        errno = 0;
        double value = strtod(buffer, &end);

        if (end == buffer) {
            printf("Invalid number\n");
            continue;
        }

        while (isspace((unsigned char)*end)) {
            end++;
        }

        if (*end != '\0' || errno == ERANGE || !isfinite(value)) {
            printf("Invalid number\n");
            continue;
        }

        return value;
    }
}

char read_command(const char *prompt, const char *allowed) {
    char buffer[256];
    char command, extra;

    while (1) {
        read_line(prompt, buffer, sizeof(buffer));

        if (sscanf(buffer, " %c %c", &command, &extra) != 1) {
            printf("Enter one command\n");
            continue;
        }

        if (strchr(allowed, command) == NULL) {
            printf("Unknown command\n");
            continue;
        }

        return command;
    }
}

int main(void) {
    char new_calculation;

    do {
        double result = read_number("Enter the first number: ");

        while (1) {
            char operation = read_command(
                "Choose operation (+, -, *, /, =): ",
                "+-*/="
            );

            if (operation == '=') {
                break;
            }

            double num = read_number("Enter the next number: ");

            if (operation == '*') {
                result *= num;
            }
            else if (operation == '/') {
                if (num == 0) {
                    printf("Cannot divide by zero\n");
                    continue;
                }

                result /= num;
            }
            else if (operation == '+') {
                result += num;
            }
            else if (operation == '-') {
                result -= num;
            }
        }

        printf("Result: %.2f\n", result);

        new_calculation = read_command(
            "Enter q to quit or y to start a new calculation: ",
            "qy"
        );

    } while (new_calculation != 'q');

    return 0;
}