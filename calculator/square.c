#include <stdio.h>


// Функция. Напиши функцию square, которая принимает double и возвращает его квадрат. Вызови её из main и выведи результат. +
double square(double num) {
    double area;
    area = num*num;
    return area;
}

int main(void) {
    double num;

    printf("enter a number:");
    scanf("%lf", &num);

    double result = square(num);



    printf("%f\n", result);

    return 0;
}




