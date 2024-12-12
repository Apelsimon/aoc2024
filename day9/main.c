#define KNUT_IMPLEMENTATION
#include "../knut.h"
#define KNUT_DS_IMPLEMENTATION
#include "../knut_ds.h"
#define KNUT_IO_IMPLEMENTATION
#include "../knut_io.h"

#include <inttypes.h>

static bool done(const knut_pair_i64_t* iterators)
{
    return iterators->first == iterators->second;
}

static void update_iterators(const knut_array_i64_t* blocks, knut_pair_i64_t* iterators)
{
    while (iterators->first < iterators->second && 
        knut_array_i64_at(blocks, iterators->first) != -1)
    {
        ++iterators->first;
    }

    while (iterators->first < iterators->second && 
        knut_array_i64_at(blocks, iterators->second) == -1)
    {
        --iterators->second;
    }
}

static knut_pair_i64_t find_iterators(const knut_array_i64_t* blocks)
{
    knut_pair_i64_t iterators = {
        0, 
        knut_array_i64_size(blocks) - 1
    };

    update_iterators(blocks, &iterators);

    return iterators;
}

static void swap_and_step(const knut_array_i64_t* blocks, knut_pair_i64_t* iterators)
{
    knut_array_i64_set(blocks, iterators->first, knut_array_i64_at(blocks, iterators->second));
    knut_array_i64_set(blocks, iterators->second, -1);
    ++iterators->first;
    --iterators->second;
}

static knut_pair_i64_t find_next_empty(const knut_array_i64_t* blocks, uint64_t start)
{
    const uint64_t size = knut_array_i64_size(blocks);
    int64_t empty_start = -1;
    
    for (uint64_t i = start; i < size; ++i)
    {
        if (knut_array_i64_at(blocks, i) == -1)
        {
            empty_start = i;
            break;
        }
    }

    if (empty_start == -1)
    {
        return (knut_pair_i64_t){ -1, -1 };
    }

    uint64_t empty_end = empty_start;

    while (empty_end < size && knut_array_i64_at(blocks, empty_end) == -1)
    {
        ++empty_end;
    }

    return (knut_pair_i64_t){ empty_start, empty_end - empty_start };
}

static knut_pair_i64_t find_last_block(const knut_array_i64_t* blocks, int64_t block_end, 
    int64_t find_less_than)
{
    KNUT_ASSERT(block_end >= 0, "[find_last_block] Can't start on negative position\n");
    #define CURRENT(i) knut_array_i64_at(blocks, i)

    while (block_end >= 0 && (CURRENT(block_end) == -1 || CURRENT(block_end) >= find_less_than))
    {
        --block_end;
    }

    if (block_end == -1)
    {
        return (knut_pair_i64_t){ -1, -1 };
    }

    const int64_t block_nr = knut_array_i64_at(blocks, block_end);
    int64_t block_start = block_end;

    while (block_start > 0 && knut_array_i64_at(blocks, block_start) == block_nr)
    {
        --block_start;
    }

    if (block_start != 0) { ++block_start; }

    return (knut_pair_i64_t){ block_start, block_end + 1 - block_start };
}

static knut_pair_i64_t find_free_memory(const knut_array_i64_t* blocks, int64_t start,
    knut_pair_i64_t block)
{
    knut_pair_i64_t free_memory;

    do
    {
        free_memory = find_next_empty(blocks, start);
        start += free_memory.second;
    } while (free_memory.first != -1 && free_memory.first < block.first && 
        free_memory.second < block.second);

    if (free_memory.first >= block.first)
    {
        return (knut_pair_i64_t){ -1, -1 };
    }
    
    return free_memory;
}

static int64_t swap_and_step_block(const knut_array_i64_t* blocks, knut_pair_i64_t free_slot, 
    knut_pair_i64_t block)
{
    KNUT_ASSERT(block.second <= free_slot.second, 
        "[swap_and_step_block] Free slot must be larger than block size\n");
    knut_pair_i64_t iterators = {free_slot.first, block.first + block.second - 1};

    for (uint64_t i = 0; i < (uint64_t)block.second; ++i)
    {
        swap_and_step(blocks, &iterators);
    }

    return iterators.second;
}

static void part_one(knut_array_i64_t* blocks)
{
    knut_pair_i64_t iterators = find_iterators(blocks);

    while (!done(&iterators))
    {
        swap_and_step(blocks, &iterators);
        update_iterators(blocks, &iterators);
    }

    const int64_t packed_size = find_iterators(blocks).first;
    uint64_t checksum = 0;

    for (uint64_t i = 0; i < (uint64_t)packed_size; ++i)
    {
        checksum += i * knut_array_i64_at(blocks, i);
    }

    printf("Part one: %" PRIu64 "\n", checksum);
}

static void part_two(knut_array_i64_t* blocks)
{
    int64_t left_ptr = 0;
    const uint64_t p2_size = knut_array_i64_size(blocks);
    int64_t right_ptr = p2_size - 1;
    int64_t current_id = INT64_MAX;

    while (left_ptr < right_ptr)
    {
        knut_pair_i64_t leftmost_empty = find_next_empty(blocks, left_ptr);
        knut_pair_i64_t rightmost_block = find_last_block(blocks, right_ptr, current_id);
        const int64_t current = knut_array_i64_at(blocks, rightmost_block.first);

        current_id = current;
        knut_pair_i64_t free_slot = find_free_memory(blocks, leftmost_empty.first, 
            rightmost_block);
        
        left_ptr = leftmost_empty.first;
        right_ptr = rightmost_block.first;
        
        if (free_slot.first != -1)
        {
            right_ptr = swap_and_step_block(blocks, free_slot, rightmost_block);
        }
        else
        {
            --right_ptr;
        }
    }

    const int64_t packed_size = find_iterators(blocks).second + 1;
    uint64_t checksum = 0;

    for (uint64_t i = 0; i < (uint64_t)packed_size; ++i)
    {
        const int64_t current = knut_array_i64_at(blocks, i);
        checksum += i * (current != -1 ? current : 0);
    }

    printf("Part two: %" PRIu64 "\n", checksum);
}

int main(int argc, char** argv)
{
    knut_exit_if(argc != 2, "Wrong number of args\n");

    knut_buffer_char_t buffer;
    knut_io_read_binary(&buffer, argv[1]);

    knut_array_i64_t blocks_p1 = knut_array_i64_create(buffer.size);

    uint64_t file_id = 0;

    for (uint64_t i = 0; i < buffer.size; i += 2)
    {
        int8_t files = buffer.ptr[i] - '0';
        int8_t free_space  = i < buffer.size - 1 ? buffer.ptr[i + 1] - '0' : -1;

        for (uint64_t j = 0; j < files; ++j)
        {
            knut_array_i64_push(&blocks_p1, file_id);
        }
        for (int64_t j = 0; j < free_space; ++j)
        {
            knut_array_i64_push(&blocks_p1, -1);
        }

        ++file_id;
    }

    knut_array_i64_t blocks_p2 = knut_array_i64_copy(&blocks_p1);

    part_one(&blocks_p1);
    part_two(&blocks_p2);

    knut_array_i64_destroy(&blocks_p2);
    knut_array_i64_destroy(&blocks_p1);
    knut_buffer_char_free(&buffer);

    return EXIT_SUCCESS;
}