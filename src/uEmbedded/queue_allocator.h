/*! \brief Implements dynamic allocatible queue. */
#pragma once
#include "allocator.h"
#include <stdlib.h>

struct queue_allocator {
    size_t head;
    size_t tail;
    size_t cap;
    size_t cnt;
    char * buff;
};

/*! \brief Initialize queue allcoator. Deinitiation of queue alloator can simply
 * be done by releasing memory on \ref queue_allocator::buff */
void queue_allocator_init(struct queue_allocator *s, void *buff,
                          size_t capacity);

/*! \brief Push new data into queue.
    \details
        First queue's head indexer will move firstly, then the data will be
   returned. Internal data will be aligned by 4-byte order. */
void *queue_allocator_push(struct queue_allocator *s, size_t size);

/*! \brief Pop data from queue. Not returns popped data. */
void queue_allocator_pop(struct queue_allocator *s);

/*! \brief Peek next data which pending to be popped on next pop() call. Data
 * size will also be returned. */
void *queue_allocator_peek(struct queue_allocator *s, size_t *size);
