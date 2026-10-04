#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <math.h>

#define TEXT_SIZE 512
#define HISTORY_SIZE 10
#define HISTORY_FILE "calculator_history.txt"
#define MAX_DEPTH 64

typedef struct {
    char expression[TEXT_SIZE];
    double result;
} Calculation;

typedef struct {
    Calculation history[HISTORY_SIZE];
    size_t count;
    double memory;
    double last_result;
} Calculator;

typedef struct {
    const char *cursor;
    const char *error;
    double memory;
    unsigned depth;
} Parser;

static int read_input(const char *prompt, char *text, size_t size);
static int evaluate(const char *expression, double memory, double *result,
                    const char **error);
static double parse_expression(Parser *parser);
static double parse_unary(Parser *parser);
static void add_history(Calculator *calculator, const char *expression, double result);
static void load_history(Calculator *calculator);
static int save_history(const Calculator *calculator);
static void show_history(const Calculator *calculator);
static void show_result(double result);
static int calculate(Calculator *calculator, const char *expression);
static int memory_command(Calculator *calculator, const char *command);
static int run_menu(Calculator *calculator);

static void skip_spaces(Parser *parser) {
    while (isspace((unsigned char)*parser->cursor)) {
        parser->cursor++;
    }
}

static double checked_result(Parser *parser, double result) {
    if (!isfinite(result) || errno == EDOM || errno == ERANGE) {
        parser->error = "Result is invalid or out of range";
        return 0;
    }
    return result;
}

static double parse_primary(Parser *parser) {
    skip_spaces(parser);
    if (parser->error) {
        return 0;
    }
    if (++parser->depth > MAX_DEPTH) {
        parser->error = "Expression is nested too deeply";
        parser->depth--;
        return 0;
    }

    double value = 0;
    if (*parser->cursor == '(') {
        parser->cursor++;
        value = parse_expression(parser);
        skip_spaces(parser);
        if (*parser->cursor != ')') {
            parser->error = "Expected closing parenthesis";
        }
        else {
            parser->cursor++;
        }
    }
    else if (strncmp(parser->cursor, "sqrt", 4) == 0) {
        parser->cursor += 4;
        skip_spaces(parser);
        if (*parser->cursor != '(') {
            parser->error = "Use sqrt(expression)";
        }
        else {
            parser->cursor++;
            double argument = parse_expression(parser);
            skip_spaces(parser);
            if (*parser->cursor != ')') {
                parser->error = "Expected closing parenthesis";
            }
            else {
                parser->cursor++;
                if (argument < 0) {
                    parser->error = "Cannot take square root of a negative number";
                }
                else if (!parser->error) {
                    errno = 0;
                    value = checked_result(parser, sqrt(argument));
                }
            }
        }
    }
    else if (strncmp(parser->cursor, "MR", 2) == 0) {
        parser->cursor += 2;
        value = parser->memory;
    }
    else {
        char *end;
        errno = 0;
        value = strtod(parser->cursor, &end);
        if (end == parser->cursor) {
            parser->error = "Expected a number, MR, sqrt(...), or parentheses";
        }
        else {
            parser->cursor = end;
            value = checked_result(parser, value);
        }
    }
    parser->depth--;
    return value;
}

static double parse_power(Parser *parser) {
    double base = parse_primary(parser);
    skip_spaces(parser);
    if (!parser->error && *parser->cursor == '^') {
        parser->cursor++;
        double exponent = parse_unary(parser);
        if (!parser->error) {
            errno = 0;
            base = checked_result(parser, pow(base, exponent));
        }
    }
    return base;
}

static double parse_unary(Parser *parser) {
    skip_spaces(parser);
    if (parser->error) {
        return 0;
    }
    if (++parser->depth > MAX_DEPTH) {
        parser->error = "Expression is nested too deeply";
        parser->depth--;
        return 0;
    }
    double value;
    if (*parser->cursor == '+' || *parser->cursor == '-') {
        char sign = *parser->cursor++;
        value = parse_unary(parser);
        if (sign == '-') {
            value = -value;
        }
    }
    else {
        value = parse_power(parser);
    }
    parser->depth--;
    return value;
}

static double parse_product(Parser *parser) {
    double value = parse_unary(parser);
    while (!parser->error) {
        skip_spaces(parser);
        char operation = *parser->cursor;
        if (operation != '*' && operation != '/' && operation != '%') {
            break;
        }
        parser->cursor++;
        double operand = parse_unary(parser);
        if (parser->error) {
            break;
        }
        if ((operation == '/' || operation == '%') && operand == 0) {
            parser->error = "Cannot divide or take remainder by zero";
            break;
        }
        errno = 0;
        switch (operation) {
            case '*': value = checked_result(parser, value * operand); break;
            case '/': value = checked_result(parser, value / operand); break;
            case '%': value = checked_result(parser, fmod(value, operand)); break;
        }
    }
    return value;
}

static double parse_expression(Parser *parser) {
    double value = parse_product(parser);
    while (!parser->error) {
        skip_spaces(parser);
        char operation = *parser->cursor;
        if (operation != '+' && operation != '-') {
            break;
        }
        parser->cursor++;
        double operand = parse_product(parser);
        if (parser->error) {
            break;
        }
        errno = 0;
        value = checked_result(parser, operation == '+' ? value + operand : value - operand);
    }
    return value;
}

static int evaluate(const char *expression, double memory, double *result,
                    const char **error) {
    Parser parser = {expression, NULL, memory, 0};
    double value = parse_expression(&parser);
    skip_spaces(&parser);
    if (!parser.error && *parser.cursor != '\0') {
        parser.error = "Unexpected text after expression";
    }
    *error = parser.error;
    if (parser.error) {
        return 0;
    }
    *result = value;
    return 1;
}

/* 1 = complete line, 0 = EOF, -1 = rejected line. */
static int read_input(const char *prompt, char *text, size_t size) {
    printf("%s", prompt);
    fflush(stdout);
    if (!fgets(text, (int)size, stdin)) {
        return 0;
    }
    char *newline = strchr(text, '\n');
    if (newline) {
        *newline = '\0';
    }
    else if (!feof(stdin)) {
        int ch;
        while ((ch = getchar()) != '\n' && ch != EOF) {
        }
        printf("Input is too long\n");
        return -1;
    }
    size_t length = strlen(text);
    while (length && isspace((unsigned char)text[length - 1])) {
        text[--length] = '\0';
    }
    char *start = text;
    while (isspace((unsigned char)*start)) {
        start++;
    }
    memmove(text, start, strlen(start) + 1);
    return 1;
}

static void add_history(Calculator *calculator, const char *expression, double result) {
    if (calculator->count == HISTORY_SIZE) {
        memmove(calculator->history, calculator->history + 1,
                (HISTORY_SIZE - 1) * sizeof(Calculation));
        calculator->count--;
    }
    Calculation *entry = &calculator->history[calculator->count++];
    snprintf(entry->expression, sizeof(entry->expression), "%s", expression);
    entry->result = result;
}

static void load_history(Calculator *calculator) {
    FILE *file = fopen(HISTORY_FILE, "r");
    if (!file) {
        if (errno != ENOENT) {
            perror("Cannot load history");
        }
        return;
    }
    char line[TEXT_SIZE + 64];
    while (fgets(line, sizeof(line), file)) {
        char *newline = strchr(line, '\n');
        if (!newline && !feof(file)) {
            int ch;
            while ((ch = fgetc(file)) != '\n' && ch != EOF) {
            }
            continue;
        }
        if (newline) {
            *newline = '\0';
        }
        char *end;
        errno = 0;
        double result = strtod(line, &end);
        if (end == line || *end != '\t' || errno == ERANGE || !isfinite(result)) {
            continue;
        }
        char *expression = end + 1;
        if (!*expression || strlen(expression) >= TEXT_SIZE) {
            continue;
        }
        add_history(calculator, expression, result);
        calculator->last_result = result;
    }
    if (ferror(file)) {
        perror("Cannot read history");
    }
    fclose(file);
}

static int save_history(const Calculator *calculator) {
    FILE *file = fopen(HISTORY_FILE, "w");
    if (!file) {
        perror("Cannot save history");
        return 0;
    }
    int success = 1;
    for (size_t i = 0; i < calculator->count; i++) {
        const Calculation *entry = &calculator->history[i];
        if (fprintf(file, "%.17g\t%s\n", entry->result, entry->expression) < 0) {
            success = 0;
            break;
        }
    }
    if (fclose(file) != 0) {
        success = 0;
    }
    if (!success) {
        fprintf(stderr, "Could not write history\n");
    }
    return success;
}

static void show_history(const Calculator *calculator) {
    if (!calculator->count) {
        printf("History is empty\n");
    }
    for (size_t i = 0; i < calculator->count; i++) {
        printf("%zu. %s = %.15g\n", i + 1,
               calculator->history[i].expression, calculator->history[i].result);
    }
}

static void show_result(double result) {
    printf("Result: %.15g\n", result);
}

static int calculate(Calculator *calculator, const char *expression) {
    double result;
    const char *error;
    if (!evaluate(expression, calculator->memory, &result, &error)) {
        fprintf(stderr, "Error: %s\n", error);
        return 0;
    }
    show_result(result);
    calculator->last_result = result;
    add_history(calculator, expression, result);
    save_history(calculator);
    return 1;
}

static int memory_command(Calculator *calculator, const char *command) {
    if (strcmp(command, "MR") == 0) {
        printf("Memory: %.15g\n", calculator->memory);
        return 1;
    }
    if (strcmp(command, "MC") == 0) {
        calculator->memory = 0;
        printf("Memory cleared\n");
        return 1;
    }
    if (strncmp(command, "M+", 2) != 0 && strncmp(command, "M-", 2) != 0) {
        return 0;
    }
    const char *expression = command + 2;
    while (isspace((unsigned char)*expression)) {
        expression++;
    }
    double value = calculator->last_result;
    const char *error;
    if (*expression && !evaluate(expression, calculator->memory, &value, &error)) {
        fprintf(stderr, "Error: %s\n", error);
        return 1;
    }
    errno = 0;
    double next_memory = command[1] == '+' ? calculator->memory + value : calculator->memory - value;
    if (!isfinite(next_memory)) {
        fprintf(stderr, "Error: memory is out of range\n");
        return 1;
    }
    calculator->memory = next_memory;
    printf("Memory: %.15g\n", calculator->memory);
    return 1;
}

static int run_menu(Calculator *calculator) {
    char command[TEXT_SIZE];
    while (1) {
        printf("\n1. Calculate\n2. Show history\n3. Clear history\n4. Exit\n"
               "Memory: M+, M-, MR, MC (M+ and M- accept an expression)\n");
        int status = read_input("Choose a command or enter an expression: ", command, sizeof(command));
        if (status == 0) {
            return 0;
        }
        if (status < 0) {
            continue;
        }
        if (memory_command(calculator, command)) {
            continue;
        }
        if (strlen(command) == 1 && command[0] >= '1' && command[0] <= '4') {
            switch (command[0]) {
                case '1':
                    status = read_input("Expression: ", command, sizeof(command));
                    if (status == 0) {
                        return 0;
                    }
                    if (status > 0) {
                        calculate(calculator, command);
                    }
                    break;
                case '2': show_history(calculator); break;
                case '3':
                    calculator->count = 0;
                    if (save_history(calculator)) {
                        printf("History cleared\n");
                    }
                    break;
                case '4': return 0;
            }
        }
        else if (strcmp(command, "q") == 0) {
            return 0;
        }
        else {
            calculate(calculator, command);
        }
    }
}

int main(int argc, char *argv[]) {
    if (argc == 2 && strcmp(argv[1], "--help") == 0) {
        printf("Usage: %s [expression]\nExample: %s 'sqrt((2 + 3) * 5)'\n"
               "Operators: + - * / %% ^; memory value: MR\n", argv[0], argv[0]);
        return 0;
    }
    Calculator calculator = {0};
    load_history(&calculator);
    if (argc > 1) {
        char expression[TEXT_SIZE] = "";
        size_t used = 0;
        for (int i = 1; i < argc; i++) {
            size_t length = strlen(argv[i]);
            size_t separator = i > 1 ? 1 : 0;
            if (length + separator >= sizeof(expression) - used) {
                fprintf(stderr, "Expression is too long\n");
                return 1;
            }
            if (separator) {
                expression[used++] = ' ';
            }
            memcpy(expression + used, argv[i], length + 1);
            used += length;
        }
        return calculate(&calculator, expression) ? 0 : 1;
    }
    return run_menu(&calculator);
}
