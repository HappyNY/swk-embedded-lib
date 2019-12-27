#pragma once

#define uassert(expr)                                                          \
    if (!(expr)) {                                                             \
        uemb_assert_impl(#expr, __FILE__, __func__, __LINE__);                 \
    }

extern void (*uemb_assert_impl)(char const *expr, char const *file,
                                char const *function, int line);