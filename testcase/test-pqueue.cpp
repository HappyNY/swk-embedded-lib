#include <Catch2/catch.hpp>
extern "C"
{
#include "uEmbedded/priority_queue.h"
}

TEST_CASE("Priority Queue", "[pqueue]")
{
    return;

    pqueue_t s;
    pqueue_init(&s, sizeof(double), malloc(0x100), 0x100,
                [](auto a, auto b) -> int { return *(double *)a - *(double *)b; });

    INFO("Capacity is " << s.capacity);

    // Push random elements
    for (size_t i = 0; i < s.capacity; i++)
    {
        double v = (double)((rand() << 16) + rand());
        pqueue_push(&s, &v);
    }

    REQUIRE(s.cnt == s.capacity);

    double maxv = std::numeric_limits<double>::min();
    while (s.cnt)
    {
        INFO("At count " << s.cnt);
        REQUIRE(maxv < *(double *)pqueue_peek(&s));
        maxv = *(double *)pqueue_peek(&s);
        pqueue_pop(&s);
    }
}