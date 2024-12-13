#define KNUT_IMPLEMENTATION
#include "../knut.h"
#define KNUT_DS_IMPLEMENTATION
#include "../knut_ds.h"
#define KNUT_IO_IMPLEMENTATION
#include "../knut_io.h"

#include <inttypes.h>
#include <math.h>

KNUT_DEFINE_ARRAY(knut_pair_u64_t, pair_u64)
KNUT_DEFINE_DEQUEUE(knut_pair_u64_t, pair_u64)


static uint64_t to_index(uint64_t x, uint64_t y, uint64_t width)
{
    return y * width + x;
}

static int16_t tile(const knut_buffer_char_t* map, int64_t x, int64_t y, int64_t width,
    int64_t height)
{
    if (0 <= x && x < width && 0 <= y && y < height)
    {
        return (int16_t)(map->ptr[to_index(x, y, width)] - '0');
    }

    return -1;
}

static void part_one(const knut_buffer_char_t* map, uint64_t width, uint64_t height)
{
    knut_buffer_bool_t visited;
    visited.size = map->size;
    visited.ptr = calloc(visited.size, sizeof(*visited.ptr));
    knut_dequeue_pair_u64_t queue = knut_dequeue_pair_u64_create(visited.size);

    #define NUM_DIRECTIONS 4
    const knut_pair_i8_t directions[NUM_DIRECTIONS] = {
        (knut_pair_i8_t){ 1, 0 },
        (knut_pair_i8_t){ 0, 1 },
        (knut_pair_i8_t){ -1, 0 },
        (knut_pair_i8_t){ 0, -1 },
    };

    uint64_t score = 0;

    for (uint64_t x = 0; x < width; ++x)
    {
        for (uint64_t y = 0; y < height; ++y)
        {
            int16_t current_tile = tile(map, x, y, width, height);

            if (current_tile == 0)
            {
                knut_dequeue_pair_u64_clear(&queue);
                memset(visited.ptr, 0, sizeof(*visited.ptr) * visited.size);
                uint64_t reached = 0;

                knut_pair_u64_t pos = { x, y };
                knut_dequeue_pair_u64_push_back(&queue, pos);
                visited.ptr[to_index(x, y, width)] = true;

                while (!knut_dequeue_pair_u64_is_empty(&queue))
                {
                    pos = knut_dequeue_pair_u64_front(&queue);
                    knut_dequeue_pair_u64_pop_front(&queue);
                    current_tile = tile(map, pos.first, pos.second, width, height);

                    if (current_tile == 9)
                    {
                        ++reached;
                    }
                    else
                    {
                        for (uint8_t i = 0; i < NUM_DIRECTIONS; ++i)
                        {
                            const knut_pair_i8_t dir = directions[i];
                            const knut_pair_i64_t neighbour = {
                                (int64_t)pos.first + dir.first,
                                (int64_t)pos.second + dir.second,
                            };
                            const int16_t neighbour_tile = tile(
                                map, neighbour.first, neighbour.second, width, height);
                            const uint64_t neighbour_index = to_index(
                                neighbour.first, neighbour.second, width);

                            if (neighbour_tile == (current_tile + 1) && 
                                !visited.ptr[neighbour_index])
                            {
                                visited.ptr[neighbour_index] = true;
                                knut_dequeue_pair_u64_push_back(&queue,
                                    (knut_pair_u64_t) {
                                    neighbour.first, neighbour.second
                                });
                            }
                        }
                    }
                }

                score += reached;
            }
        }
    }

    printf("Part one: %" PRIu64 "\n", score);

    knut_dequeue_pair_u64_destroy(&queue);
    knut_buffer_bool_destroy(&visited);
}


static void search(const knut_buffer_char_t* map, uint64_t width, uint64_t height, 
    knut_pair_i64_t current_pos, uint64_t* num_found)
{
    const int16_t current_tile = tile(map, current_pos.first, current_pos.second, width, height);

    if (current_tile == -1)
    {
        return;
    }

    if (current_tile == 9)
    {
        ++(*num_found);
        return;
    }

    #define NUM_DIRECTIONS 4
    const knut_pair_i8_t directions[NUM_DIRECTIONS] = {
        (knut_pair_i8_t){ 1, 0 },
        (knut_pair_i8_t){ 0, 1 },
        (knut_pair_i8_t){ -1, 0 },
        (knut_pair_i8_t){ 0, -1 },
    };

    for (uint8_t i = 0; i < NUM_DIRECTIONS; ++i)
    {
        const knut_pair_i8_t dir = directions[i];
        const knut_pair_i64_t neighbour = {
            (int64_t)current_pos.first + dir.first,
            (int64_t)current_pos.second + dir.second,
        };

        const int16_t neighbour_tile = tile(
            map, neighbour.first, neighbour.second, width, height);

        if (neighbour_tile == (current_tile + 1))
        {
            search(map, width, height, neighbour, num_found);
        }
    }
}

static void part_two(const knut_buffer_char_t* map, uint64_t width, uint64_t height)
{
    uint64_t score = 0;

    for (uint64_t x = 0; x < width; ++x)
    {
        for (uint64_t y = 0; y < height; ++y)
        {
            int16_t current_tile = tile(map, x, y, width, height);

            if (current_tile == 0)
            {
                uint64_t num_found = 0;
                search(map, width, height, (knut_pair_i64_t){ x, y }, &num_found);
                score += num_found;
            }
        }
    }

    printf("Part two: %" PRIu64 "\n", score);
}

int main(int argc, char** argv)
{
    knut_exit_if(argc != 2, "Wrong number of args\n");

    knut_buffer_char_t map;
    knut_io_read_binary(&map, argv[1]);

    const uint64_t width = strchr(map.ptr, '\n') - map.ptr + 1;
    const uint64_t height = (uint64_t)ceill(map.size / (double)width);

    part_one(&map, width, height);
    part_two(&map, width, height);

    knut_buffer_char_destroy(&map);

    return EXIT_SUCCESS;
}