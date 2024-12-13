#define KNUT_IMPLEMENTATION
#include "../knut.h"
#define KNUT_DS_IMPLEMENTATION
#include "../knut_ds.h"
#define KNUT_IO_IMPLEMENTATION
#include "../knut_io.h"

#include <inttypes.h>
#include <math.h>

static int64_t coordinate(int64_t x, int64_t y, int64_t width)
{
    return y * width + x;
}

static bool outof_bounds(int64_t x, int64_t y, int64_t width, int64_t height)
{
    return x < 0 || y < 0 || x >= width || y >= height;
}

static char sample(const char* grid, int64_t x, int64_t y, int64_t width, int64_t height)
{
    if (outof_bounds(x, y, width, height))
    {
        return ' ';
    }

    return grid[coordinate(x, y, width)];
}

static knut_pair_i64_t find_guard(const char* grid, int64_t width, int64_t height)
{
    knut_pair_i64_t p;

    for (int64_t x = 0; x < width; ++x)
    {
        for (int64_t y = 0; y < height; ++y)
        {
            if (sample(grid, x, y, width, height) == '^')
            {
                p.first = x;
                p.second = y;
                return p;                
            }
        }
    }

    KNUT_ASSERT(false, "[find_guard] Unable to find guard\n");
    return (knut_pair_i64_t){};
}

typedef struct {
    knut_pair_i64_t pos;
    knut_pair_i8_t dir;
} guard_t;

static bool guard_outof_bounds(guard_t* guard, int64_t width, int64_t height)
{
    const knut_pair_i64_t pos = guard->pos;
    return outof_bounds(pos.first, pos.second, width, height);
}

static void walk(guard_t* guard, const char* grid, int64_t width, int64_t height)
{
    const knut_pair_i64_t pos = guard->pos;
    const knut_pair_i8_t dir = guard->dir;

    const char s = sample(grid, pos.first + dir.first, pos.second + dir.second, width, height);

    if (s == '#')
    {
        guard->dir.first = dir.first == 0 ? -1 * dir.second : 0;
        guard->dir.second = dir.second == 0 ? dir.first : 0;
        guard->pos.first += guard->dir.first;
        guard->pos.second += guard->dir.second;
    }
    else
    {
        guard->pos.first += dir.first;
        guard->pos.second += dir.second;
    }
}

int main(int argc, char** argv)
{
    knut_exit_if(argc != 2, "Wrong number of args\n");

    knut_buffer_char_t buffer;
    KNUT_ASSERT(knut_io_read_binary(&buffer, argv[1]) != -1, "Failed to read file\n");

    const int64_t width = strchr(buffer.ptr, '\n') - buffer.ptr + 1;
    const int64_t height = (int64_t)ceill(buffer.size / (double)width);

    const knut_pair_i64_t start_pos = find_guard(buffer.ptr, width, height);
    const knut_pair_i8_t start_dir = { 0, -1 };
    guard_t guard = {
        start_pos,
        start_dir
    };

    buffer.ptr[coordinate(start_pos.first, start_pos.second, width)] = 'X';
    uint64_t positions = 1;

    while (!guard_outof_bounds(&guard, width, height))
    {
        walk(&guard, buffer.ptr, width, height);
        const char current = sample(buffer.ptr, guard.pos.first, guard.pos.second, width, height);

        if (current != 'X' && current != ' ')
        {
            buffer.ptr[coordinate(guard.pos.first, guard.pos.second, width)] = 'X';
            ++positions;
        }
    }

    printf("Part one: %" PRIu64 "\n", positions);

    knut_buffer_char_destroy(&buffer);

    return EXIT_SUCCESS;
}