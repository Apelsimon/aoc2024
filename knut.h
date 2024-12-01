#ifndef KNUT_INCLUDE_H
#define KNUT_INCLUDE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

uint16_t knut_swap_u16(uint16_t val);
uint32_t knut_swap_u32(uint32_t val);

void knut_exit_if(bool condition, const char* msg);

typedef struct {
    void (*proc)(void*);
    void* arg;
} knut_function_t;

void knut_exit_if_with_cleanup(bool condition, const char* msg, knut_function_t cleanup_cb);

#ifdef KNUT_NOASSERT
#define KNUT_ASSERT(condition, msg) ((void*)0)
#else
#define KNUT_ASSERT(condition, msg) knut_exit_if(!(condition), msg)
#endif

char* knut_strndup(const char* str, size_t size);

int64_t knut_clamp_i64(int64_t value, int64_t min, int64_t max);

#define KNUT_DEFINE_ARRAY(Type) \
typedef struct { \
    Type* buffer; \
    uint64_t size; \
    uint64_t capacity; \
} knut_array_##Type##_t; \
\
/* TODO: make this part of struct above */ \
typedef struct { \
    Type* buffer; \
    uint64_t size; \
} knut_array_##Type##_data_t; \
\
static void knut_array_##Type##_init(knut_array_##Type##_t* array, uint64_t capacity) \
{ \
    array->buffer = (Type*)calloc(capacity, sizeof(*array->buffer)); \
    KNUT_ASSERT(array->buffer, "[knut_array_" #Type "_init] Failed to alloc buffer\n"); \
    array->size = 0; \
    array->capacity = capacity; \
} \
\
static knut_array_##Type##_t knut_array_##Type##_create(uint64_t capacity) \
{ \
    knut_array_##Type##_t array; \
    knut_array_##Type##_init(&array, capacity); \
    return array; \
} \
\
static void knut_array_##Type##_destroy(knut_array_##Type##_t* array) \
{ \
    free(array->buffer); \
    memset(array, 0, sizeof(*array)); \
} \
\
static void knut_array_##Type##_clear(knut_array_##Type##_t* array) \
{ \
    array->size = 0; \
} \
\
static void knut_array_##Type##_push_slice(knut_array_##Type##_t* array, const Type* entries, \
    uint64_t num_entries) \
{ \
    const size_t value_size = sizeof(*array->buffer); \
 \
    while (array->capacity < (array->size + num_entries)) \
    { \
        KNUT_ASSERT( \
            array->capacity <= (UINT64_MAX / 2), \
            "[knut_array_" #Type "_push] Capacity overflow" \
        ); \
        array->capacity *= 2; \
        array->buffer = (Type*)realloc(array->buffer, value_size * array->capacity); \
    } \
 \
    memcpy(array->buffer + array->size, entries, value_size * num_entries); \
    array->size += num_entries; \
} \
\
static void knut_array_##Type##_push(knut_array_##Type##_t* array, Type f) \
{ \
    knut_array_##Type##_push_slice(array, &f, 1); \
} \
\
static void knut_array_##Type##_pop(knut_array_##Type##_t* array) \
{ \
    KNUT_ASSERT(array->size > 0, "[knut_array_" #Type "_pop] Can't pop empty array"); \
    --array->size; \
} \
\
static void knut_array_##Type##_pop_slice(knut_array_##Type##_t* array, uint64_t size) \
{ \
    KNUT_ASSERT(array->size >= size, "[knut_array_" #Type "_pop_slice] Can't pop slice of \
        that size"); \
    array->size -= size; \
} \
\
static uint64_t knut_array_##Type##_size(const knut_array_##Type##_t* array) \
{ \
    return array->size; \
} \
\
static bool knut_array_##Type##_is_empty(const knut_array_##Type##_t* array) \
{ \
    return array->size == 0; \
} \
\
static uint64_t knut_array_##Type##_capacity(const knut_array_##Type##_t* array) \
{ \
    return array->capacity; \
} \
\
static Type knut_array_##Type##_at(const knut_array_##Type##_t* array, uint64_t pos) \
{ \
    KNUT_ASSERT(pos < array->size, "[knut_array_" #Type "_at] Index out of bounds"); \
    return array->buffer[pos]; \
} \
static void knut_array_##Type##_set(const knut_array_##Type##_t* array, uint64_t pos, Type t) \
{ \
    KNUT_ASSERT(pos < array->size, "[knut_array_" #Type "_set] Index out of bounds"); \
    array->buffer[pos] = t; \
} \
\
static knut_array_##Type##_data_t knut_array_##Type##_get_data(const knut_array_##Type##_t* array) \
{ \
    return (knut_array_##Type##_data_t){ array->buffer, array->size }; \
}\
\

KNUT_DEFINE_ARRAY(float)
KNUT_DEFINE_ARRAY(int)
KNUT_DEFINE_ARRAY(uint32_t)

#define KNUT_DEFINE_DEQUEUE(Type) \
typedef struct { \
    Type* buffer; \
    uint64_t capacity; \
    uint64_t size; \
    uint64_t front; \
    uint64_t back; \
} knut_dequeue_##Type##_t; \
\
static knut_dequeue_##Type##_t knut_dequeue_##Type##_create(uint64_t capacity) \
{ \
    KNUT_ASSERT(capacity > 0, "[knut_dequeue_" #Type "_create] Capacity can't be 0\n"); \
    knut_dequeue_##Type##_t dequeue = { \
        (Type*)calloc(capacity, sizeof(*dequeue.buffer)), \
        capacity, \
        0,  \
        0, \
        0 \
    }; \
 \
    return dequeue; \
} \
\
static void knut_dequeue_##Type##_destroy(knut_dequeue_##Type##_t* dequeue) \
{ \
    free(dequeue->buffer); \
    memset(dequeue, 0, sizeof(*dequeue)); \
} \
static uint64_t knut_dequeue_##Type##_size(const knut_dequeue_##Type##_t* dequeue) \
{ \
    return dequeue->size; \
} \
\
static uint64_t knut_dequeue_##Type##_capacity(const knut_dequeue_##Type##_t* dequeue) \
{ \
    return dequeue->capacity; \
} \
\
static bool knut_dequeue_##Type##_is_empty(const knut_dequeue_##Type##_t* dequeue) \
{ \
    return dequeue->size == 0; \
} \
\
static void knut_dequeue_##Type##_realloc(knut_dequeue_##Type##_t* dequeue, bool offset_new_buffer) \
{ \
    KNUT_ASSERT( \
        dequeue->capacity <= (UINT64_MAX / 2), \
        "[knut_dequeue_" #Type "_push_front] Capacity overflow" \
    ); \
 \
    const uint8_t new_buffer_offset = offset_new_buffer ? 1 : 0; \
    const uint64_t old_capacity = dequeue->capacity; \
    const uint64_t new_capacity = dequeue->capacity * 2; \
    Type* new_buffer = (Type*)calloc(new_capacity, sizeof(*dequeue->buffer)); \
    const uint64_t first_block_size = old_capacity - dequeue->front; \
    const size_t value_size = sizeof(*dequeue->buffer); \
    memcpy(new_buffer + new_buffer_offset, dequeue->buffer + dequeue->front,  \
        first_block_size * value_size); \
    uint64_t second_block_size = 0; \
 \
    if (dequeue->front != 0) \
    { \
        second_block_size = dequeue->back + 1; \
        memcpy(new_buffer + new_buffer_offset + first_block_size, dequeue->buffer,  \
            second_block_size * value_size); \
    } \
     \
    knut_dequeue_##Type##_destroy(dequeue); \
    dequeue->buffer = new_buffer; \
    dequeue->capacity = new_capacity; \
    dequeue->size = first_block_size + second_block_size; \
    dequeue->front = 0; \
    dequeue->back = dequeue->size - 1 + new_buffer_offset; \
} \
 \
static void knut_dequeue_##Type##_push_front(knut_dequeue_##Type##_t* dequeue, Type f) \
{ \
    if (knut_dequeue_##Type##_is_empty(dequeue)) \
    { \
        ++dequeue->size; \
        dequeue->buffer[0] = f; \
        return; \
    } \
 \
    const uint64_t old_front = dequeue->front; \
    uint64_t new_front = old_front > 0 ? old_front - 1 : dequeue->capacity - 1; \
 \
    if (new_front == dequeue->back) \
    { \
        knut_dequeue_##Type##_realloc(dequeue, true); \
        new_front = 0; \
    } \
 \
    ++dequeue->size; \
    dequeue->buffer[new_front] = f; \
    dequeue->front = new_front; \
} \
 \
static Type knut_dequeue_##Type##_front(const knut_dequeue_##Type##_t* dequeue) \
{ \
    KNUT_ASSERT(!knut_dequeue_##Type##_is_empty(dequeue), \
        "[knut_dequeue_" #Type "_front] Dequeue is empty"); \
    return dequeue->buffer[dequeue->front]; \
} \
 \
static void knut_dequeue_##Type##_pop_front(knut_dequeue_##Type##_t* dequeue) \
{ \
    KNUT_ASSERT(!knut_dequeue_##Type##_is_empty(dequeue), \
        "[knut_dequeue_" #Type "_pop_front] Dequeue is empty"); \
    --dequeue->size; \
    const uint64_t current_front = dequeue->front; \
    dequeue->front = current_front < (dequeue->capacity - 1) ? current_front + 1 : 0; \
 \
    if (knut_dequeue_##Type##_is_empty(dequeue)) \
    { \
        dequeue->front = 0; \
        dequeue->back = 0; \
    } \
} \
 \
static void knut_dequeue_##Type##_push_back(knut_dequeue_##Type##_t* dequeue, Type f) \
{ \
    if (knut_dequeue_##Type##_is_empty(dequeue)) \
    { \
        ++dequeue->size; \
        dequeue->buffer[0] = f; \
        return; \
    } \
 \
    const uint64_t old_back = dequeue->back; \
    uint64_t new_back = old_back < (dequeue->capacity - 1) ? old_back + 1 : 0; \
 \
    if (new_back == dequeue->front) \
    { \
        knut_dequeue_##Type##_realloc(dequeue, false); \
        new_back = dequeue->size; \
    } \
 \
    ++dequeue->size; \
    dequeue->buffer[new_back] = f; \
    dequeue->back = new_back; \
} \
 \
static Type knut_dequeue_##Type##_back(const knut_dequeue_##Type##_t* dequeue) \
{ \
    KNUT_ASSERT(!knut_dequeue_##Type##_is_empty(dequeue), \
        "[knut_dequeue_" #Type "_back] Dequeue is empty"); \
    return dequeue->buffer[dequeue->back]; \
} \
 \
static void knut_dequeue_##Type##_pop_back(knut_dequeue_##Type##_t* dequeue) \
{ \
    KNUT_ASSERT(!knut_dequeue_##Type##_is_empty(dequeue), \
        "[knut_dequeue_" #Type "_pop_back] Dequeue is empty"); \
    --dequeue->size; \
    const uint64_t current_back = dequeue->back; \
    dequeue->back = current_back > 0 ? current_back - 1 : dequeue->capacity - 1; \
 \
    if (knut_dequeue_##Type##_is_empty(dequeue)) \
    { \
        dequeue->front = 0; \
        dequeue->back = 0; \
    } \
} \
 \
static void knut_dequeue_##Type##_foreach(knut_dequeue_##Type##_t* dequeue, void(*callback)(Type*)) \
{ \
    if (knut_dequeue_##Type##_is_empty(dequeue)) \
    { \
        return; \
    } \
 \
    if (dequeue->front > dequeue->back) \
    { \
        for (uint64_t i = dequeue->front; i < dequeue->capacity; ++i) \
        { \
            callback(&dequeue->buffer[i]); \
        } \
 \
        for (uint64_t i = 0; i <= dequeue->back; ++i) \
        { \
            callback(&dequeue->buffer[i]); \
        } \
    } \
    else \
    { \
        for (uint64_t i = dequeue->front; i <= dequeue->back; ++i) \
        { \
            callback(&dequeue->buffer[i]); \
        } \
    } \
} \

KNUT_DEFINE_DEQUEUE(float)
KNUT_DEFINE_DEQUEUE(int)

#ifdef __cplusplus
}
#endif

#endif // KNUT_INCLUDE_H

// ==============================================================================
// ==============================================================================
// ==============================================================================
// ==============================================================================
// ==============================================================================
// ==============================================================================

#if defined(KNUT_IMPLEMENTATION) && !defined(KNUT_IMPLEMENTATION_DONE)
#define KNUT_IMPLEMENTATION_DONE

#ifdef __cplusplus
extern "C" {
#endif

uint16_t knut_swap_u16(uint16_t val)
{
    return val >> 8 | ((val & 0xFF) << 8);
}

uint32_t knut_swap_u32(uint32_t val)
{
    return val >> 24                |
        ((val & 0x00FF0000) >> 8)   |
        ((val & 0x0000FF00) << 8)   |
        ((val & 0xFF) << 24);
}

void knut_exit_if(bool condition, const char* msg)
{
    if (condition)
    {
        fprintf(stderr, msg);
        fflush(stderr);
        exit(EXIT_FAILURE);
    }
}

void knut_exit_if_with_cleanup(bool condition, const char* msg, knut_function_t cleanup_cb)
{
    if (condition)
    {
        fprintf(stderr, msg);
        fflush(stderr);
        cleanup_cb.proc(cleanup_cb.arg);
        exit(EXIT_FAILURE);
    }
}

char* knut_strndup(const char* str, size_t size)
{
    char* dup_str = (char*)calloc(size + 1, sizeof(*str));
    knut_exit_if(dup_str == NULL, "[knut_strndup] calloc failed\n");
    strncpy_s(dup_str, size + 1, str, size);
    return dup_str;
}

int64_t knut_clamp_i64(int64_t value, int64_t min, int64_t max)
{
    if (value < min) { return min; }
    if (value > max) { return max; }
    return value;
}

#ifdef __cplusplus
}
#endif

#endif // KNUT_IMPLEMENTATION