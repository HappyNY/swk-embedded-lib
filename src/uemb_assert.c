#pragma once
#include <assert.h>

void uemb_assert_impl_default( char const* expr, char const* file, char const* function, int line )
{
    assert( 0 && expr );
}

void ( *uemb_assert_impl )( char const* expr, char const* file, char const* function, int line ) = uemb_assert_impl_default;
