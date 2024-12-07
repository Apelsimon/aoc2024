#define KNUT_IMPLEMENTATION
#include "../knut.h"
#define KNUT_DS_IMPLEMENTATION
#include "../knut_ds.h"

#include <inttypes.h>
#include <math.h>

static void part_one(const knut_array_int_data_t* left_numbers, 
    const knut_array_int_data_t* right_numbers)
{
    uint64_t total_distance = 0;
    const uint64_t list_size = left_numbers->size;

    for (uint64_t i = 0; i < list_size; ++i)
    {
        total_distance += (uint64_t)fabs(left_numbers->buffer[i] - right_numbers->buffer[i]);
    }
    
    printf("Part one: %" PRIu64 "\n", total_distance);
}

static void part_two(const knut_array_int_data_t* left_numbers, 
    const knut_array_int_data_t* right_numbers)
{
    const uint64_t list_size = left_numbers->size;
    uint64_t total_similarity_score = 0;
    int prev_number = 0;
    uint64_t prev_similarity = 0;
    uint64_t j = 0;

    for (uint64_t i = 0; i < list_size; ++i)
    {
        uint64_t similarity = 0;
        int current_number = left_numbers->buffer[i];

        if (i != 0 && prev_number == current_number)
        {
            similarity = prev_similarity;
        }
        else
        {
            while (j < right_numbers->size && current_number > right_numbers->buffer[j]) { ++j; }
            while (j < right_numbers->size && right_numbers->buffer[j] == current_number) 
            { 
                ++similarity;
                ++j;
            }

            similarity *= current_number;
        }

        total_similarity_score += similarity;
        prev_number = current_number;
        prev_similarity = similarity;
    }

    printf("Part two: %" PRIu64 "\n", total_similarity_score);
}

static int compare_ints(const void* a, const void* b)
{
    return *(int*)a - *(int*)b;
}

int main(int argc, char** argv)
{
    knut_exit_if(argc != 2, "Wrong number of args\n");

    FILE* file = fopen(argv[1], "r");
    knut_exit_if(!file, "Unable to open file\n");

    const uint8_t LIST_CAPACITY = 64;
    knut_array_int_t left_list = knut_array_int_create(LIST_CAPACITY);
    knut_array_int_t right_list = knut_array_int_create(LIST_CAPACITY);

    bool push_left = true;
    int number;
    while (fscanf_s(file, "%d", &number) == 1) 
    {
        if (push_left)
        {
            knut_array_int_push(&left_list, number);
        }
        else
        {
            knut_array_int_push(&right_list, number);
        }

        push_left = !push_left;
    }

    fclose(file);

    knut_array_int_data_t left_numbers = knut_array_int_get_data(&left_list);
    knut_array_int_data_t right_numbers = knut_array_int_get_data(&right_list);

    knut_exit_if(left_numbers.size != right_numbers.size, "List sizes not matching\n");

    qsort(left_numbers.buffer, left_numbers.size, sizeof(*left_numbers.buffer), compare_ints);
    qsort(right_numbers.buffer, right_numbers.size, sizeof(*right_numbers.buffer), compare_ints);

    part_one(&left_numbers, &right_numbers);
    part_two(&left_numbers, &right_numbers);

    return EXIT_SUCCESS;
}