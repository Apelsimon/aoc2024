#define KNUT_IMPLEMENTATION
#include "../knut.h"

static int part_one(int* report_numbers, uint8_t report_numbers_size)
{
    KNUT_ASSERT(report_numbers_size >= 2, "Too few numbers\n");
    const bool increasing = report_numbers[1] > report_numbers[0];

    for (int i = 0; i < report_numbers_size - 1; ++i)
    {
        const int diff = report_numbers[i + 1] - report_numbers[i];
        const int abs_diff = abs(diff);

        if (abs_diff < 1 || abs_diff > 3 || (increasing && diff < 0) || 
            (!increasing && diff > 0))
        {
            return 0;
        }
    }

    return 1;
}

static int test_damped_buffer(int* report_numbers, uint8_t report_numbers_size, int* tmp_buffer, 
    int index_to_exclude)
{
    const int i = index_to_exclude;
    memcpy(tmp_buffer, report_numbers, i * sizeof(*tmp_buffer));
    memcpy(tmp_buffer + i, report_numbers + i + 1, 
        (report_numbers_size - (i + 1)) * sizeof(*tmp_buffer));
    return part_one(tmp_buffer, report_numbers_size - 1);
}

static int part_two(int* report_numbers, uint8_t report_numbers_size)
{
    KNUT_ASSERT(report_numbers_size >= 2, "Too few numbers\n");
    const bool increasing = report_numbers[1] > report_numbers[0];

    for (int i = 0; i < report_numbers_size - 1; ++i)
    {
        const int diff = report_numbers[i + 1] - report_numbers[i];
        const int abs_diff = abs(diff);

        if (abs_diff < 1 || abs_diff > 3 || (increasing && diff < 0) || 
            (!increasing && diff > 0))
        {
            int* damped_buffer = (int*)calloc(report_numbers_size - 1, sizeof(*damped_buffer));

            if (test_damped_buffer(report_numbers, report_numbers_size, damped_buffer, i) ||
                test_damped_buffer(report_numbers, report_numbers_size, damped_buffer, i + 1) ||
                i > 0 && test_damped_buffer(
                    report_numbers, report_numbers_size, damped_buffer, i - 1))
            {
                free(damped_buffer);
                return 1;
            }

            free(damped_buffer);
            return 0;
        }
    }

    return 1;
}

int main(int argc, char** argv)
{
    knut_exit_if(argc != 2, "Wrong number of args\n");

    FILE* file = fopen(argv[1], "r");
    knut_exit_if(!file, "Unable to open file\n");

    #define BUFFER_SIZE 32
    char report_input[BUFFER_SIZE] = {0};
    int report_numbers[BUFFER_SIZE] = {0};
    uint8_t report_numbers_size = 0;
    int total_num_safe_reports[2] = {0};

    while (fgets(report_input, BUFFER_SIZE, file))
    {
        char* input = report_input;
        report_numbers_size = 0;

        while (report_numbers_size < BUFFER_SIZE && 
            sscanf_s(input, "%d", &report_numbers[report_numbers_size]) == 1)
        {
            ++report_numbers_size;

            while (*input != ' ' && *input != '\0')
            {
                ++input;
            }

            if (*input == ' ') { ++input; }
        }

        total_num_safe_reports[0] += part_one(report_numbers, report_numbers_size);
        total_num_safe_reports[1] += part_two(report_numbers, report_numbers_size);
    }

    fclose(file);

    printf("Part one: %d\n", total_num_safe_reports[0]);
    printf("Part two: %d\n", total_num_safe_reports[1]);

    return EXIT_SUCCESS;
}