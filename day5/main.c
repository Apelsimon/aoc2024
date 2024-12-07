#define KNUT_IMPLEMENTATION
#include "../knut.h"
#define KNUT_DS_IMPLEMENTATION
#include "../knut_ds.h"

#include <inttypes.h>
#include <stdio.h>

typedef struct {
    knut_array_u16_t* rules;
    uint16_t nr;
} page_t;

KNUT_DEFINE_ARRAY(page_t, page)

int sort_pages(const void* p1, const void* p2)
{
    const page_t page1 = *(const page_t*)p1;
    const page_t page2 = *(const page_t*)p2;
 
    knut_array_u16_t* rule1 = &page1.rules[page1.nr];
    knut_array_u16_t* rule2 = &page2.rules[page2.nr];
    const uint64_t rule1_size = knut_array_u16_size(rule1);

    for (uint64_t i = 0; i < rule1_size; ++i)
    {
        if (knut_array_u16_at(rule1, i) == page2.nr)
        {
            return -1;
        }
    }

    const uint64_t rule2_size = knut_array_u16_size(rule2);

    for (uint64_t i = 0; i < rule2_size; ++i)
    {
        if (knut_array_u16_at(rule2, i) == page1.nr)
        {
            return 1;
        }
    }

    return 0;
}

static bool is_equal(knut_array_page_data_t* page_data, knut_array_u16_data_t* page_numbers_data)
{
    for (uint64_t i = 0; i < page_data->size; ++i)
    {
        if (page_data->buffer[i].nr != page_numbers_data->buffer[i])
        {
            return false;
        }
    }

    return true;
}

int main(int argc, char** argv)
{
    knut_exit_if(argc != 2, "Wrong number of args\n");

    FILE* file = fopen(argv[1], "r");
    KNUT_ASSERT(file, "Unable to open file\n");

    #define BUFFER_SIZE 1024

    knut_array_u16_t rules[BUFFER_SIZE];

    for (uint16_t i = 0; i < BUFFER_SIZE; ++i)
    {
        rules[i] = knut_array_u16_create(0);
    }

    char buffer[BUFFER_SIZE];
    bool parse_first_part = true;

    knut_array_u16_t page_numbers = knut_array_u16_create(8);
    knut_array_page_t pages = knut_array_page_create(8);
    uint32_t middle_page_numbers_p1 = 0;
    uint32_t middle_page_numbers_p2 = 0;

    while (fgets(buffer, BUFFER_SIZE, file)) 
    {
        const bool input_split = strstr(buffer, "\n") == buffer;
        parse_first_part &= !input_split;

        if (parse_first_part)
        {
            const knut_pair_u32_t pair = knut_parse_pair_u32(buffer, 10);
            KNUT_ASSERT(pair.first < BUFFER_SIZE, "Can't fit number in rules map\n");
            knut_array_u16_push(&rules[pair.first], pair.second);
        }
        else if (!input_split)
        {
            knut_array_u16_clear(&page_numbers);
            knut_array_page_clear(&pages);

            char* start = buffer;
            char* delim;
            uint32_t nr;

            while ((nr = strtol(start, &delim, 10)) != 0)
            {
                knut_array_u16_push(&page_numbers, nr);
                page_t p = { rules, nr };
                knut_array_page_push(&pages, p);
                start = delim + 1;
            }

            knut_array_page_data_t page_data = knut_array_page_get_data(&pages);
            qsort(page_data.buffer, page_data.size, sizeof(*page_data.buffer), sort_pages);

            knut_array_u16_data_t page_numbers_data = knut_array_u16_get_data(&page_numbers);

            if (is_equal(&page_data, &page_numbers_data))
            {
                middle_page_numbers_p1 += page_numbers_data.buffer[page_numbers_data.size / 2];
            }
            else
            {
                middle_page_numbers_p2 += page_data.buffer[page_data.size / 2].nr;
            }
        }
    }

    fclose(file);

    printf("Part one: %" PRIu32 "\n", middle_page_numbers_p1);
    printf("Part two: %" PRIu32 "\n", middle_page_numbers_p2);

    return EXIT_SUCCESS;
}