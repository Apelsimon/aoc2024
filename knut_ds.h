#ifndef KNUT_DS_INCLUDE_H
#define KNUT_DS_INCLUDE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#define KNUT_DEFINE_ARRAY(TYPE, TYPE_NAME) \
typedef struct { \
    TYPE* buffer; \
    uint64_t size; \
    uint64_t capacity; \
} knut_array_##TYPE_NAME##_t; \
\
/* TODO: make this part of struct above */ \
typedef struct { \
    TYPE* buffer; \
    uint64_t size; \
} knut_array_##TYPE_NAME##_data_t; \
\
static void knut_array_##TYPE_NAME##_init(knut_array_##TYPE_NAME##_t* array, uint64_t capacity) \
{ \
    array->buffer = (TYPE*)calloc(capacity, sizeof(*array->buffer)); \
    KNUT_ASSERT(array->buffer, "[knut_array_" #TYPE_NAME "_init] Failed to alloc buffer\n"); \
    array->size = 0; \
    array->capacity = capacity; \
} \
\
static knut_array_##TYPE_NAME##_t knut_array_##TYPE_NAME##_create(uint64_t capacity) \
{ \
    knut_array_##TYPE_NAME##_t array; \
    knut_array_##TYPE_NAME##_init(&array, capacity); \
    return array; \
} \
\
static void knut_array_##TYPE_NAME##_destroy(knut_array_##TYPE_NAME##_t* array) \
{ \
    free(array->buffer); \
    memset(array, 0, sizeof(*array)); \
} \
\
static void knut_array_##TYPE_NAME##_clear(knut_array_##TYPE_NAME##_t* array) \
{ \
    array->size = 0; \
} \
\
static void knut_array_##TYPE_NAME##_push_slice(knut_array_##TYPE_NAME##_t* array, \
    const TYPE* entries, uint64_t num_entries) \
{ \
    const size_t value_size = sizeof(*array->buffer); \
 \
    while (array->capacity < (array->size + num_entries)) \
    { \
        KNUT_ASSERT( \
            array->capacity <= (UINT64_MAX / 2), \
            "[knut_array_" #TYPE_NAME "_push] Capacity overflow" \
        ); \
        if (array->capacity == 0) \
        { \
            array->capacity = 4; \
        } \
        const uint64_t old_capacity = array->capacity; \
        array->capacity = old_capacity == 0 ? 8 : 2 * old_capacity; \
        array->buffer = (TYPE*)realloc(array->buffer, value_size * array->capacity); \
    } \
 \
    memcpy(array->buffer + array->size, entries, value_size * num_entries); \
    array->size += num_entries; \
} \
\
static void knut_array_##TYPE_NAME##_push(knut_array_##TYPE_NAME##_t* array, TYPE f) \
{ \
    knut_array_##TYPE_NAME##_push_slice(array, &f, 1); \
} \
\
static void knut_array_##TYPE_NAME##_pop(knut_array_##TYPE_NAME##_t* array) \
{ \
    KNUT_ASSERT(array->size > 0, "[knut_array_" #TYPE_NAME "_pop] Can't pop empty array"); \
    --array->size; \
} \
\
static void knut_array_##TYPE_NAME##_pop_slice(knut_array_##TYPE_NAME##_t* array, uint64_t size) \
{ \
    KNUT_ASSERT(array->size >= size, "[knut_array_" #TYPE_NAME "_pop_slice] Can't pop slice of \
        that size"); \
    array->size -= size; \
} \
\
static uint64_t knut_array_##TYPE_NAME##_size(const knut_array_##TYPE_NAME##_t* array) \
{ \
    return array->size; \
} \
\
static bool knut_array_##TYPE_NAME##_is_empty(const knut_array_##TYPE_NAME##_t* array) \
{ \
    return array->size == 0; \
} \
\
static uint64_t knut_array_##TYPE_NAME##_capacity(const knut_array_##TYPE_NAME##_t* array) \
{ \
    return array->capacity; \
} \
\
static TYPE knut_array_##TYPE_NAME##_at(const knut_array_##TYPE_NAME##_t* array, uint64_t pos) \
{ \
    KNUT_ASSERT(pos < array->size, "[knut_array_" #TYPE_NAME "_at] Index out of bounds"); \
    return array->buffer[pos]; \
} \
static void knut_array_##TYPE_NAME##_set(const knut_array_##TYPE_NAME##_t* array, \
    uint64_t pos, TYPE t) \
{ \
    KNUT_ASSERT(pos < array->size, "[knut_array_" #TYPE_NAME "_set] Index out of bounds"); \
    array->buffer[pos] = t; \
} \
\
static knut_array_##TYPE_NAME##_data_t knut_array_##TYPE_NAME##_get_data( \
    const knut_array_##TYPE_NAME##_t* array) \
{ \
    knut_array_##TYPE_NAME##_data_t data = { array->buffer, array->size }; \
    return data; \
}\
\

KNUT_DEFINE_ARRAY(float, float)
KNUT_DEFINE_ARRAY(int, int)
KNUT_DEFINE_ARRAY(uint8_t, u8)
KNUT_DEFINE_ARRAY(uint16_t, u16)
KNUT_DEFINE_ARRAY(uint32_t, u32)
KNUT_DEFINE_ARRAY(uint64_t, u64)

#define KNUT_DEFINE_DEQUEUE(TYPE, TYPE_NAME) \
typedef struct { \
    TYPE* buffer; \
    uint64_t capacity; \
    uint64_t size; \
    uint64_t front; \
    uint64_t back; \
} knut_dequeue_##TYPE_NAME##_t; \
\
static knut_dequeue_##TYPE_NAME##_t knut_dequeue_##TYPE_NAME##_create(uint64_t capacity) \
{ \
    KNUT_ASSERT(capacity > 0, "[knut_dequeue_" #TYPE_NAME "_create] Capacity can't be 0\n"); \
    knut_dequeue_##TYPE_NAME##_t dequeue = { \
        (TYPE*)calloc(capacity, sizeof(*dequeue.buffer)), \
        capacity, \
        0,  \
        0, \
        0 \
    }; \
 \
    return dequeue; \
} \
\
static void knut_dequeue_##TYPE_NAME##_destroy(knut_dequeue_##TYPE_NAME##_t* dequeue) \
{ \
    free(dequeue->buffer); \
    memset(dequeue, 0, sizeof(*dequeue)); \
} \
static uint64_t knut_dequeue_##TYPE_NAME##_size(const knut_dequeue_##TYPE_NAME##_t* dequeue) \
{ \
    return dequeue->size; \
} \
\
static uint64_t knut_dequeue_##TYPE_NAME##_capacity(const knut_dequeue_##TYPE_NAME##_t* dequeue) \
{ \
    return dequeue->capacity; \
} \
\
static bool knut_dequeue_##TYPE_NAME##_is_empty(const knut_dequeue_##TYPE_NAME##_t* dequeue) \
{ \
    return dequeue->size == 0; \
} \
\
static void knut_dequeue_##TYPE_NAME##_realloc(knut_dequeue_##TYPE_NAME##_t* dequeue, \
    bool offset_new_buffer) \
{ \
    KNUT_ASSERT( \
        dequeue->capacity <= (UINT64_MAX / 2), \
        "[knut_dequeue_" #TYPE_NAME "_push_front] Capacity overflow" \
    ); \
 \
    const uint8_t new_buffer_offset = offset_new_buffer ? 1 : 0; \
    const uint64_t old_capacity = dequeue->capacity; \
    const uint64_t new_capacity = dequeue->capacity * 2; \
    TYPE* new_buffer = (TYPE*)calloc(new_capacity, sizeof(*dequeue->buffer)); \
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
    knut_dequeue_##TYPE_NAME##_destroy(dequeue); \
    dequeue->buffer = new_buffer; \
    dequeue->capacity = new_capacity; \
    dequeue->size = first_block_size + second_block_size; \
    dequeue->front = 0; \
    dequeue->back = dequeue->size - 1 + new_buffer_offset; \
} \
 \
static void knut_dequeue_##TYPE_NAME##_push_front(knut_dequeue_##TYPE_NAME##_t* dequeue, TYPE f) \
{ \
    if (knut_dequeue_##TYPE_NAME##_is_empty(dequeue)) \
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
        knut_dequeue_##TYPE_NAME##_realloc(dequeue, true); \
        new_front = 0; \
    } \
 \
    ++dequeue->size; \
    dequeue->buffer[new_front] = f; \
    dequeue->front = new_front; \
} \
 \
static TYPE knut_dequeue_##TYPE_NAME##_front(const knut_dequeue_##TYPE_NAME##_t* dequeue) \
{ \
    KNUT_ASSERT(!knut_dequeue_##TYPE_NAME##_is_empty(dequeue), \
        "[knut_dequeue_" #TYPE_NAME "_front] Dequeue is empty"); \
    return dequeue->buffer[dequeue->front]; \
} \
 \
static void knut_dequeue_##TYPE_NAME##_pop_front(knut_dequeue_##TYPE_NAME##_t* dequeue) \
{ \
    KNUT_ASSERT(!knut_dequeue_##TYPE_NAME##_is_empty(dequeue), \
        "[knut_dequeue_" #TYPE_NAME "_pop_front] Dequeue is empty"); \
    --dequeue->size; \
    const uint64_t current_front = dequeue->front; \
    dequeue->front = current_front < (dequeue->capacity - 1) ? current_front + 1 : 0; \
 \
    if (knut_dequeue_##TYPE_NAME##_is_empty(dequeue)) \
    { \
        dequeue->front = 0; \
        dequeue->back = 0; \
    } \
} \
 \
static void knut_dequeue_##TYPE_NAME##_push_back(knut_dequeue_##TYPE_NAME##_t* dequeue, TYPE f) \
{ \
    if (knut_dequeue_##TYPE_NAME##_is_empty(dequeue)) \
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
        knut_dequeue_##TYPE_NAME##_realloc(dequeue, false); \
        new_back = dequeue->size; \
    } \
 \
    ++dequeue->size; \
    dequeue->buffer[new_back] = f; \
    dequeue->back = new_back; \
} \
 \
static TYPE knut_dequeue_##TYPE_NAME##_back(const knut_dequeue_##TYPE_NAME##_t* dequeue) \
{ \
    KNUT_ASSERT(!knut_dequeue_##TYPE_NAME##_is_empty(dequeue), \
        "[knut_dequeue_" #TYPE_NAME "_back] Dequeue is empty"); \
    return dequeue->buffer[dequeue->back]; \
} \
 \
static void knut_dequeue_##TYPE_NAME##_pop_back(knut_dequeue_##TYPE_NAME##_t* dequeue) \
{ \
    KNUT_ASSERT(!knut_dequeue_##TYPE_NAME##_is_empty(dequeue), \
        "[knut_dequeue_" #TYPE_NAME "_pop_back] Dequeue is empty"); \
    --dequeue->size; \
    const uint64_t current_back = dequeue->back; \
    dequeue->back = current_back > 0 ? current_back - 1 : dequeue->capacity - 1; \
 \
    if (knut_dequeue_##TYPE_NAME##_is_empty(dequeue)) \
    { \
        dequeue->front = 0; \
        dequeue->back = 0; \
    } \
} \
 \
static void knut_dequeue_##TYPE_NAME##_foreach(knut_dequeue_##TYPE_NAME##_t* dequeue, \
    void(*callback)(TYPE*)) \
{ \
    if (knut_dequeue_##TYPE_NAME##_is_empty(dequeue)) \
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

KNUT_DEFINE_DEQUEUE(float, float)
KNUT_DEFINE_DEQUEUE(int, int)

#ifdef __cplusplus
}
#endif

#endif // KNUT_DS_INCLUDE_H

// ==============================================================================
// ==============================================================================
// ==============================================================================
// ==============================================================================
// ==============================================================================
// ==============================================================================

#if defined(KNUT_DS_IMPLEMENTATION) && !defined(KNUT_DS_IMPLEMENTATION_DONE)
#define KNUT_DS_IMPLEMENTATION_DONE

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif

#endif // KNUT_DS_IMPLEMENTATION