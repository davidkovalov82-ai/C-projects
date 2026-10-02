#include <stdio.h>

int main(void) {
    double num, result;
    char operation, new_calculation;

    do {
        printf("Enter a number: ");
        if (scanf("%lf", &result) != 1) {
            printf("Invalid number\n");
        }
        printf("Choose operation (+, -, *, /, =): ");
        if (scanf(" %c", &operation) != 1) {
            printf("Invalid operation\n");
        }
        while (operation != '=') {
            printf("Enter a number: ");
            if (scanf("%lf", &num) != 1) {
                printf("Invalid number\n");
            }

            if (operation == '*') {
                result *= num;
            }
            else if (operation == '/') {
                if (num == 0) {
                    printf("Cannot divide by zero\n");
                    return 1;
                }
                result /= num;
            }
            else if (operation == '+') {
                result += num;
            }
            else if (operation == '-') {
                result -= num;
            }

            printf("Choose operation (+, -, *, /, =): ");
            if (scanf(" %c", &operation) != 1) {
                printf("Invalid operation\n");
            }
        }

        printf("Result: %.2f\n", result);

        printf("Enter q to quit or y to start a new calculation: ");
        if (scanf(" %c", &new_calculation) != 1) {
            printf("Invalid input\n");
        }
        scanf(" %c", &new_calculation);

    } while (new_calculation != 'q');

    return 0;
}