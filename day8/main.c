#define KNUT_IMPLEMENTATION
#include "../knut.h"
#define KNUT_DS_IMPLEMENTATION
#include "../knut_ds.h"
#define KNUT_IO_IMPLEMENTATION
#include "../knut_io.h"

#include <inttypes.h>

KNUT_DEFINE_ARRAY(knut_pair_i64_t, pair_i64)

static uint64_t remove_char(knut_buffer_char_t* buffer, char to_remove)
{
    uint64_t j = 0;

    for (uint64_t i = 0; i < buffer->size; ++i)
    {
        if (buffer->ptr[i] != to_remove)
        {
            buffer->ptr[j++] = buffer->ptr[i];
        }
    }

    const uint64_t removed = buffer->size - j;
    buffer->size = j;

    return removed;
}

static int64_t coordinate(int64_t x, int64_t y, int64_t width)
{
    return y * width + x;
}

static within_bounds(knut_pair_i64_t p, knut_pair_i64_t bounds)
{
    return 0 <= p.first && p.first < bounds.first && 0 <= p.second && p.second < bounds.second;
}

static knut_buffer_char_t copy_buffer(const knut_buffer_char_t* b)
{
    knut_buffer_char_t b_new = {
        calloc(b->size, sizeof(*b->ptr)),
        b->size
    };
    memcpy(b_new.ptr, b->ptr, b->size * sizeof(*b->ptr));
    
    return b_new;
}

static bool equal(knut_pair_i64_t p1, knut_pair_i64_t p2)
{
    return p1.first == p2.first && p1.second == p2.second;
}

static knut_pair_i64_t add(knut_pair_i64_t p1, knut_pair_i64_t p2)
{
    return (knut_pair_i64_t){
        p1.first + p2.first,
        p1.second + p2.second
    };
}

int main(int argc, char** argv)
{
    knut_exit_if(argc != 2, "Wrong number of args\n");

    knut_buffer_char_t buffer_p1;
    knut_io_read_binary(&buffer_p1, argv[1]);

    const char* newline = strchr(buffer_p1.ptr, '\n');
    KNUT_ASSERT(newline, "Expected newline\n");

    const int64_t width = (int64_t)(newline - buffer_p1.ptr);
    const int64_t height = (int64_t)(remove_char(&buffer_p1, '\n') + 1);
    const knut_pair_i64_t grid_size = { width, height };

    knut_buffer_char_t buffer_p2 = copy_buffer(&buffer_p1);

    #define ASCII_MAP_SIZE 123
    knut_array_pair_i64_t ascii_positions[123];

    for (uint8_t c = 0; c < ASCII_MAP_SIZE; ++c)
    {
        ascii_positions[c] = knut_array_pair_i64_create(0);
    }

    for (int64_t y = 0; y < height; ++y)
    {
        for (int64_t x = 0; x < width; ++x)
        {
            const char c = buffer_p1.ptr[coordinate(x, y, width)];

            if (isdigit(c) || isalpha(c))
            {
                knut_array_pair_i64_push(&ascii_positions[c], (knut_pair_i64_t){ x, y });
            }
        }
    }

    uint64_t total_p1 = 0;
    uint64_t total_p2 = 0;

    for (uint8_t c = 0; c < ASCII_MAP_SIZE; ++c)
    {
        const knut_array_pair_i64_t* positions = &ascii_positions[c];
        const uint64_t size = knut_array_pair_i64_size(positions);

        if (size > 0)
        {
            for (uint64_t i = 0; i < size; ++i)
            {
                for (uint64_t j = 0; j < size; ++j)
                {
                    if (i == j) { continue; }

                    const knut_pair_i64_t p1 = knut_array_pair_i64_at(positions, i);
                    const knut_pair_i64_t p2 = knut_array_pair_i64_at(positions, j);
                    const knut_pair_i64_t direction = add(
                        p2, 
                        (knut_pair_i64_t){-p1.first, -p1.second}
                    );

                    {
                        const knut_pair_i64_t p_new = add(p2, direction);
                        const bool within = within_bounds(p_new, grid_size);
                        const int64_t coord = coordinate(p_new.first, p_new.second, width);

                        if (within && buffer_p1.ptr[coord] != '#')
                        {
                            buffer_p1.ptr[coord] = '#';
                            total_p1 += 1;
                        }
                    }

                    {
                        knut_pair_i64_t p_new = p2;

                        while (within_bounds(p_new, grid_size))
                        {
                            const int64_t coord = coordinate(p_new.first, p_new.second, width);

                            if (buffer_p2.ptr[coord] != '#')
                            {
                                buffer_p2.ptr[coord] = '#';
                                total_p2 += 1;
                            }

                            p_new = add(p_new, direction);
                        }
                    }
                }
            }
        }
    }

    printf("Part one: %" PRIu64 "\n", total_p1);
    printf("Part two: %" PRIu64 "\n", total_p2);

    knut_buffer_char_destroy(&buffer_p1);
    knut_buffer_char_destroy(&buffer_p2);

    return EXIT_SUCCESS;
}