#define KNUT_IMPLEMENTATION
#include "../knut.h"

#include <inttypes.h>

typedef struct {
    char* buffer;
    uint64_t size;
} string_t;

static string_t read_file(FILE* file, uint64_t max_size)
{
    fseek(file, 0, SEEK_END);
    const uint64_t size = ftell(file);
    knut_exit_if(max_size < size + 1, "File too big\n");
    rewind(file);

    char* buffer = malloc(size + 1);

    const size_t bytes_read = fread(buffer, sizeof(*buffer), size, file);
    knut_exit_if(bytes_read != size, "Unable to read whole file\n");

    buffer[bytes_read] = '\0';

    return (string_t){buffer, bytes_read};
}

uint64_t remove_char(string_t* str, char to_remove)
{
    uint64_t j = 0;

    for (uint64_t i = 0; i < str->size; ++i)
    {
        if (str->buffer[i] != to_remove)
        {
            str->buffer[j++] = str->buffer[i];
        }
    }

    const uint64_t removed = str->size - j;

    str->buffer[j] = '\0';
    str->size = j;

    return removed;
}

static bool intersects_linebreak(uint32_t index, uint32_t num_columns)
{
    return index % num_columns > (num_columns - 4);
}

static bool starts_with(const char* start, const char* with)
{
    return strstr(start, with) == start;
}

static uint32_t coordinate(uint32_t row, uint32_t column, uint32_t num_columns)
{
    return row * num_columns + column;
}

static void part_one(const string_t* str, uint32_t num_columns, uint32_t num_rows)
{
    uint32_t num_horizontal = 0;

    for (uint32_t i = 0; i < str->size; ++i)
    {
        if (str->buffer[i] != '.')
        {
            const char* current = str->buffer + i;
            const bool got_match = starts_with(current, "XMAS") || starts_with(current, "SAMX");
            num_horizontal += got_match && !intersects_linebreak(i, num_columns) ? 1 : 0;
        }
    }

    uint32_t num_vertical = 0;

    for (uint32_t c = 0; c < num_columns; ++c)
    {
        for (uint32_t r = 0; r < num_rows - 3; ++r)
        {
            if (str->buffer[coordinate(r, c, num_columns)] != '.')
            {
                const char word[] = { 
                    str->buffer[coordinate(r, c, num_columns)],
                    str->buffer[coordinate(r + 1, c, num_columns)],
                    str->buffer[coordinate(r + 2, c, num_columns)],
                    str->buffer[coordinate(r + 3, c, num_columns)],
                    '\0'
                };

                const bool got_match = starts_with(word, "XMAS") || starts_with(word, "SAMX");
                num_vertical += got_match ? 1 : 0;
            }
        }
    }

    uint32_t num_diagonal = 0;

    uint32_t start_r = num_rows - 4;
    uint32_t start_c = 0;
    bool scanning_left_bottom = true;

    while (start_c < num_columns)
    {
        uint32_t r = start_r;
        uint32_t c = start_c;

        for (uint32_t r = start_r, c = start_c; r < num_rows - 3 && c < num_columns - 3; 
            ++r, ++c)
        {
            if (str->buffer[coordinate(r, c, num_columns)] != '.')
            {
                const char word[] = { 
                    str->buffer[coordinate(r, c, num_columns)],
                    str->buffer[coordinate(r + 1, c + 1, num_columns)],
                    str->buffer[coordinate(r + 2, c + 2, num_columns)],
                    str->buffer[coordinate(r + 3, c + 3, num_columns)],
                    '\0'
                };

                const bool got_match = starts_with(word, "XMAS") || starts_with(word, "SAMX");
                num_diagonal += got_match ? 1 : 0;
            }
        }

        if (scanning_left_bottom)
        {
            --start_r;
            scanning_left_bottom = start_r > 0;
        }
        else
        {
            ++start_c;
        }
    }

    start_r = 3;
    start_c = 0;
    bool scanning_top_left = true;

    while (start_c < num_columns)
    {
        uint32_t r = start_r;
        uint32_t c = start_c;

        for (uint32_t r = start_r, c = start_c; r > 2 && c < num_columns - 3; --r, ++c)
        {
            if (str->buffer[coordinate(r, c, num_columns)] != '.')
            {
                const char word[] = { 
                    str->buffer[coordinate(r, c, num_columns)],
                    str->buffer[coordinate(r - 1, c + 1, num_columns)],
                    str->buffer[coordinate(r - 2, c + 2, num_columns)],
                    str->buffer[coordinate(r - 3, c + 3, num_columns)],
                    '\0'
                };

                const bool got_match = starts_with(word, "XMAS") || starts_with(word, "SAMX");
                num_diagonal += got_match ? 1 : 0;
            }
        }

        if (scanning_top_left)
        {
            ++start_r;
            scanning_top_left = start_r < num_rows - 1;
        }
        else
        {
            ++start_c;
        }
    }

    printf("Part one: %" PRIu32 "\n", num_horizontal + num_vertical + num_diagonal);
}

static void part_two(const string_t* str, uint32_t num_columns, uint32_t num_rows)
{
    uint32_t total = 0;

    for (uint32_t r = 0; r < num_rows - 2; ++r)
    {
        for (uint32_t c = 0; c < num_columns - 2; ++c)
        {
            const char diag1[] = { 
                str->buffer[coordinate(r, c, num_columns)],
                str->buffer[coordinate(r + 1, c + 1, num_columns)],
                str->buffer[coordinate(r + 2, c + 2, num_columns)],
                '\0'
            };
            const char diag2[] = { 
                str->buffer[coordinate(r + 2, c, num_columns)],
                str->buffer[coordinate(r + 1, c + 1, num_columns)],
                str->buffer[coordinate(r, c + 2, num_columns)],
                '\0'
            };

            const bool got_match = (starts_with(diag1, "MAS") || starts_with(diag1, "SAM")) && (starts_with(diag2, "MAS") || starts_with(diag2, "SAM"));
            total += got_match ? 1 : 0;
        }
    }

    printf("Part two: %" PRIu32 "\n", total);
}

int main(int argc, char** argv)
{
    knut_exit_if(argc != 2, "Wrong number of args\n");

    FILE* file = fopen(argv[1], "rb");
    knut_exit_if(!file, "Unable to open file\n");

    string_t str = read_file(file, (1 << 16) - 1);
    fclose(file);

    const char* newline = strchr(str.buffer, '\n');
    knut_exit_if(!newline, "Expected newline in input\n");

    const uint32_t num_columns = (uint32_t)(newline - str.buffer);
    const uint32_t num_rows = (uint32_t)(remove_char(&str, '\n') + 1);

    part_one(&str, num_columns, num_rows);
    part_two(&str, num_columns, num_rows);

    return EXIT_SUCCESS;
}