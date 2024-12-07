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

char* knut_strndup(const char* str, uint64_t size);

int64_t knut_clamp_i64(int64_t value, int64_t min, int64_t max);

#define KNUT_DEFINE_PAIR(TYPE, TYPE_NAME) \
    typedef struct { \
        TYPE first; \
        TYPE second; \
    } knut_pair_##TYPE_NAME##_t; \

KNUT_DEFINE_PAIR(uint32_t, u32)

knut_pair_u32_t knut_parse_pair_u32(const char* input, int base);

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

char* knut_strndup(const char* str, uint64_t size)
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

knut_pair_u32_t knut_parse_pair_u32(const char* input, int base)
{
    knut_pair_u32_t pair;

    char* delim = NULL;
    pair.first = strtol(input, &delim, base);
    KNUT_ASSERT(delim, "[knut_parse_pair_u32] No delimiter found\n");
    pair.second = strtol(delim + 1, NULL, base);

    return pair;
}

#ifdef __cplusplus
}
#endif

#endif // KNUT_IMPLEMENTATION