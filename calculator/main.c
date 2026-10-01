#include <stdio.h>

int main(void) {
    double num, result;
    char operation, new_calculation;

    do {
        printf("Enter a number: ");
        scanf("%lf", &result);

        printf("Choose operation (+, -, *, /, =): ");
        scanf(" %c", &operation);

        while (operation != '=') {
            printf("Enter a number: ");
            scanf("%lf", &num);

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
            else {
                printf("Unknown operation\n");
                return 1;
            }

            printf("Choose operation (+, -, *, /, =): ");
            scanf(" %c", &operation);
        }

        printf("Result: %.2f\n", result);

        printf("Enter q to quit or y to start a new calculation: ");
        scanf(" %c", &new_calculation);

    } while (new_calculation != 'q');

    return 0;
}