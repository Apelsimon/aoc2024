#define KNUT_IMPLEMENTATION
#include "../knut.h"
#define KNUT_DS_IMPLEMENTATION
#include "../knut_ds.h"

#include <inttypes.h>
#include <math.h>
#include <stdio.h>
#include <string.h>

static uint64_t concat_numbers(uint64_t left, uint64_t right)
{
    uint64_t num_digits = 0;
    uint64_t r = right;

    while (r > 0)
    {
        r /= 10;
        ++num_digits;
    }

    return left * (uint64_t)powl(10, (double)num_digits) + right;
}

static bool valid_equation(const knut_array_u64_t* numbers, uint64_t number_index, 
    uint64_t target_sum, uint64_t current_sum, bool use_concat)
{
    if (current_sum > target_sum)
    {
        return false;
    }

    if (knut_array_u64_size(numbers) == number_index)
    {
        return current_sum == target_sum;
    }

    const uint64_t current_number = knut_array_u64_at(numbers, number_index);
    const uint64_t next_index = number_index + 1;
    return 
        valid_equation(numbers, next_index, target_sum, current_number + current_sum, use_concat) ||
        valid_equation(numbers, next_index, target_sum, current_number * current_sum, use_concat) ||
        (use_concat && valid_equation(numbers, next_index, target_sum, 
            concat_numbers(current_sum, current_number), use_concat));
}

int main(int argc, char** argv)
{
    knut_exit_if(argc != 2, "Wrong number of args\n");

    FILE* file = fopen(argv[1], "rb");
    #define BUFFER_SIZE 1024
    char buffer[BUFFER_SIZE] = {0};

    knut_array_u64_t numbers = knut_array_u64_create(8);
    uint64_t total_p1 = 0;
    uint64_t total_p2 = 0;

    while (fgets(buffer, BUFFER_SIZE, file))
    {
        const uint64_t target_sum = atoll(buffer);

        const char* delim = " ";
        char *next_token;
        char *token = strtok_s(strchr(buffer, ':') + 1, delim, &next_token);

        knut_array_u64_clear(&numbers);

        while (token)
        {
            knut_array_u64_push(&numbers, atoll(token));
            token = strtok_s(NULL, delim, &next_token);
        }

        if (valid_equation(&numbers, 1, target_sum, knut_array_u64_at(&numbers, 0), false))
        {
            total_p1 += target_sum;
        }

        if (valid_equation(&numbers, 1, target_sum, knut_array_u64_at(&numbers, 0), true))
        {
            total_p2 += target_sum;
        }
    }

    printf("Part one: %" PRIu64 "\n", total_p1);
    printf("Part two: %" PRIu64 "\n", total_p2);

    knut_array_u64_destroy(&numbers);
    fclose(file);

    return EXIT_SUCCESS;
}