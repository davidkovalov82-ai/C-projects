#include <stdio.h>

int main(void) {
    double first_num, second_num, result;
    char operation;
    printf("Enter a first number: ");
    scanf("%lf", &first_num);

    printf("Choose operation: ");
    scanf(" %c", &operation);

    printf("Enter a second number: ");
    scanf("%lf", &second_num);

    if (operation == '*') {
        result = first_num * second_num;
    }
    else if (operation == '/') {
        if (second_num == 0) {
            printf("Cannot divide by zero\n");
            return 1;
        }
        result = first_num / second_num;
    }
    else if (operation == '+') {
        result = first_num + second_num;
    }
    else if (operation == '-') {
        result = first_num - second_num;
    }
    else {
        printf("Unknown operation\n");
        return 1;
    }


    printf("%lf %c %lf = %lf\n", first_num, operation, second_num, result);

    return 0;
}