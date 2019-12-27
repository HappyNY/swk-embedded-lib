#include <assert.h>
#include <stdio.h>

void uemb_assert_impl_default(char const *expr, char const *file,
                              char const *function, int line)
{
    fprintf(stderr, "<<< ASSERT >>> \n\t%s() on line %d: %s \n\t EXPR ... %s",
            function, line, file, expr);
    assert(0 && expr);
}

void (*uemb_assert_impl)(char const *expr, char const *file,
                         char const *function,
                         int         line) = uemb_assert_impl_default;
