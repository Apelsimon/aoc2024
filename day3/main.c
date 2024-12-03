#define KNUT_IMPLEMENTATION
#include "../knut.h"

#include <ctype.h>
#include <stdbool.h>

static is_mul_char(char c)
{
    return c == 'm' ||
        c == 'u' ||
        c == 'l' ||
        c == '(' ||
        c == ')' ||
        c == ',' ||
        isdigit(c) != 0;
}

static bool parsing_mul(char* stack, uint8_t stack_top)
{
    return stack_top > 0 && stack[0] == 'm';
}

static is_do_dont_char(char c)
{
    return c == 'd' ||
        c == 'o' ||
        c == 'n' ||
        c == '\'' ||
        c == 't' ||
        c == '(' ||
        c == ')';
}

static bool parsing_do_dont(char* stack, uint8_t stack_top)
{
    return stack_top > 0 && stack[0] == 'd';
}

#define MAX_STACK_SIZE 64

static void stack_push(char* stack, uint8_t* stack_top, char c)
{
    knut_exit_if(*stack_top >= MAX_STACK_SIZE, "stack overflow\n");
    stack[(*stack_top)++] = c;
}

static void stack_clear(char* stack, uint8_t* stack_top)
{
    stack[0] = 0;
    *stack_top = 0;
}

int main(int argc, char** argv)
{
    knut_exit_if(argc != 2, "Wrong number of args\n");

    FILE* file = fopen(argv[1], "r");
    knut_exit_if(!file, "Unable to open file\n");

    char stack[MAX_STACK_SIZE] = {0};
    uint8_t stack_top = 0;
    bool do_mul = true;
    int total_part_one = 0;
    int total_part_two = 0;

    int cint;
    while ((cint = fgetc(file)) != EOF)
    {
        const char c = (char)cint;

        if (parsing_mul(stack, stack_top) && is_mul_char(c))
        {
            stack_push(stack, &stack_top, c);

            if (c == ')')
            {
                stack_push(stack, &stack_top, '\0');

                int left, right;
                if (sscanf_s(stack, "mul(%d,%d)", &left, &right) == 2)
                {
                    const int product = left * right;
                    total_part_one += product;
                    total_part_two += (do_mul ? 1 : 0) * product;
                }

                stack_clear(stack, &stack_top);
            }
        }
        else if (parsing_do_dont(stack, stack_top) && is_do_dont_char(c))
        {
            stack_push(stack, &stack_top, c);

            if (c == ')')
            {
                stack_push(stack, &stack_top, '\0');

                if (strstr(stack, "do()"))
                {
                    do_mul = true;
                }
                if (strstr(stack, "don't()"))
                {
                    do_mul = false;
                }

                stack_clear(stack, &stack_top);
            }
        }
        else if (stack_top == 0 && (c == 'm' || c == 'd'))
        {
            stack_push(stack, &stack_top, c);
        }
        else
        {
            stack_clear(stack, &stack_top);
        }
    }

    printf("Part one: %d\n", total_part_one);
    printf("Part one: %d\n", total_part_two);

    fclose(file);


    return EXIT_SUCCESS;
}