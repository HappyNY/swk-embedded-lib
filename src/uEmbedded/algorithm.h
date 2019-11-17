#pragma once
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

/*! \breif      Find lowest index to insert [eval] parameter for sorted array. 
    \note       [eval] and [arr] can be different type of object if you define predicate function in that way. Left side of predicate function will hold [eval]. */
size_t lowerbound(void const* arr, void const* eval, size_t elemSize, size_t numElems, int ( *pred )( void const* /*eval*/, void const* ));

