#pragma once
#include <assert.h>
#include "assert.h"

void uemb_assert_impl( char const* expr, char const* file, char const* function, int line )
{
    assert( 0 && expr );
}
