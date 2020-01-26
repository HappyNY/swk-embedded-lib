#pragma once
#include "macro.h"
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

/*! \brief Allocator descriptor. */
typedef struct allocator {
    void *(*allocate)(void *, size_t);
    void (*release)(void *, void *);
    void *object;
} allocator_t;

typedef allocator_t const *allocator_ref_t;

//! Default allocator instance using malloc().
extern allocator_t const g_mallocator;

#ifdef __cplusplus
}
#endif