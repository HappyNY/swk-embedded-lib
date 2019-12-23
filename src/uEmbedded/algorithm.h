#pragma once
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

/*! \brief      Find lowest index to insert [eval] parameter for sorted array. 
    \note       [eval] and [arr] can be different type of object if you define predicate function in that way. Left side of predicate function will hold [eval]. */
size_t lowerbound(void const *arr, void const *eval, size_t elemSize, size_t numElems, int (*pred)(void const * /*eval*/, void const *));

/*! \brief Insert new element to linear array
    \param arr          Array reference
    \param elem         Element reference. Set NULL to 
    \param index        Index to insert new argument
    \param elemSize     Size of element
    \param lpNumElems   Number of element
    \returns Pointer to newly inserted item.
    \warning Maximum number of elements must be larger than numElems!
 */
void *array_insert(void const *arr, void const *elem, size_t index, size_t elemSize, size_t *lpNumElems);
