# C Calculator

A small educational calculator written in C.

## Current features

- Input of two `double` values
- Addition, subtraction, multiplication, and division
- Operation selection with `switch`
- Division-by-zero protection
- Unknown-operation handling

## Example

```text
Enter the first number: 5
Choose an operation (+, -, *, /): /
Enter the second number: 2
5.00 / 2.00 = 2.50
```

## Build and run in CLion

Open the project directory in CLion and run the `Calculator` configuration.

## Development ideas

| Done | Feature | Expected behavior | C concepts |
|:---:|---|---|---|
| ✅ | Basic calculator | Calculate one operation with two numbers | Variables, `double`, `char`, `scanf`, `printf`, `if` or `switch` |
| ✅ | Chained calculations | Accept any number of values and operations until the user enters `=` | `while`, `break`, accumulated result |
| ✅ | Multiple calculations | Start another calculation without restarting the program; exit with `q` | `do while`, nested loops |
| ✅ | Input validation | Handle text, empty input, and malformed numbers | Return value of `scanf`, input-buffer cleanup |
| ✅ | Additional operations | Add remainder, power, and square root | `%`, `math.h`, `pow`, `sqrt` |
| ✅ | Functions | Move input, calculation, and output into separate functions | Parameters, return values, function prototypes |
| ✅ | History | Keep the last 10 calculations in memory | Arrays, structures, strings |
| ✅ | Menu | Add calculate, show history, clear history, and exit commands | Loops, `switch`, program organization |
| ✅ | Saved history | Save calculations to a file and load them at startup | `FILE`, `fopen`, `fprintf`, `fgets`, `fclose` |
| ✅ | Command-line mode | Support commands such as `./Calculator 10 + 5` | `argc`, `argv`, `strtod` |
| ✅ | Calculator memory | Add `M+`, `M-`, `MR`, and `MC` commands | Program state, functions |
| ✅ | Full expressions | Evaluate expressions such as `(2 + 3) * 4` with normal operator precedence | Parsing, precedence, stacks or recursion |

## Planned chained-calculation mode

The calculator should accept as many numbers and operations as the user wants:

```text
Enter the first number: 10
Choose an operation (+, -, *, /, =): +
Enter the next number: 5
Choose an operation (+, -, *, /, =): *
Enter the next number: 2
Choose an operation (+, -, *, /, =): -
Enter the next number: 3
Choose an operation (+, -, *, /, =): =
Result: 27.00
```

This first implementation will evaluate operations from left to right:

```text
((10 + 5) * 2) - 3 = 27
```

Standard mathematical precedence and parentheses are covered by the **Full expressions** idea.

## Suggested learning order

1. Chained calculations
2. Input validation
3. Functions
4. Multiple calculations
5. History
6. Menu
7. Saved history
8. Command-line mode

The remaining ideas can be added whenever you want to expand the calculator's mathematical features.
