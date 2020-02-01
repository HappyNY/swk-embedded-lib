#include "allocator.h"

static void* malloc_wrapper( void* placeholder, size_t sz )
{
    return malloc( sz );
}

static void free_wrapper( void* placeholder, void* mem )
{
    free( mem );
}

allocator_t const g_mallocator = { malloc_wrapper, free_wrapper, NULL };