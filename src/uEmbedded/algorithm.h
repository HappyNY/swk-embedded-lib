#pragma once
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

size_t lower_bound(void const* arr, size_t elemSize, size_t numElems, int ( *pred )( void const*, void const* ));