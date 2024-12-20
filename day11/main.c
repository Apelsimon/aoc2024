#define KNUT_IMPLEMENTATION
#include "../knut.h"
#define KNUT_DS_IMPLEMENTATION
#include "../knut_ds.h"

#include <inttypes.h>
#include <math.h>

typedef struct {
    uint64_t nr;
    uint64_t num_blinks;
} stone_t;

KNUT_DEFINE_DEQUEUE(stone_t, stone)

static uint64_t num_digits(uint64_t nr)
{
    uint64_t count = 1;
    uint64_t pow = 10;
    while (pow <= nr) { pow *= 10; ++count; }
    return count;
}

static bool split_number(knut_pair_u32_t* numbers, uint64_t nr)
{
    const uint64_t digit_count = num_digits(nr);

    if (digit_count % 2 != 0)
    {
        return false;
    }

    const uint32_t div = (uint32_t)powl(10, (long double)(digit_count / 2));
    const uint32_t bottom = nr % div;
    const uint32_t upper = (uint32_t)((nr - (uint64_t)bottom) / (uint64_t)div);

    numbers->first = upper;
    numbers->second = bottom;

    return true;
}

int main(int argc, char** argv)
{
    knut_exit_if(argc != 2, "Wrong number of args\n");

    knut_dequeue_stone_t stones = knut_dequeue_stone_create(1024);

    FILE* file = fopen(argv[1], "rb");

    uint64_t total_stones = 0;
    const uint8_t max_blinks = 25;
    uint64_t nr;

    while (fscanf_s(file, "%" PRIu64, &nr) == 1)
    {
        knut_dequeue_stone_push_back(&stones, (stone_t){ nr, 0 });
    }

    while (!knut_dequeue_stone_is_empty(&stones))
    {
        stone_t stone = knut_dequeue_stone_front(&stones);
        knut_dequeue_stone_pop_front(&stones);

        if (stone.num_blinks == max_blinks)
        {
            ++total_stones;
            continue;
        }

        ++stone.num_blinks;
        const uint64_t stone_nr = stone.nr;
        knut_pair_u32_t number_split;

        if (stone_nr == 0)
        {
            stone.nr = 1;
        }
        else if (split_number(&number_split, stone_nr))
        {
            stone_t new_stone = { number_split.second, stone.num_blinks };
            knut_dequeue_stone_push_front(&stones, new_stone);
            stone.nr = number_split.first;
        }
        else
        {
            stone.nr *= 2024;
        }

        knut_dequeue_stone_push_front(&stones, stone);
    }

    printf("Part one: %" PRIu64 "\n", total_stones);
    
    fclose(file);
    knut_dequeue_stone_destroy(&stones);

    return EXIT_SUCCESS;
}