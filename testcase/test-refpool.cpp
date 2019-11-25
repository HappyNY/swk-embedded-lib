#include <Catch2/catch.hpp>
extern "C"
{
#include <uEmbedded/managed_reference_pool.h>
}

TEST_CASE("refpool test", "[managed_reference_pool]")
{
    managed_reference_pool_t* p;
    enum { NUM_MAX_REF = 10000 };
    p = refpool_create(NUM_MAX_REF);

    refhandle_t hd[NUM_MAX_REF];

    int idx = 0;
    for ( auto& h : hd )
    {
        h = refpool_malloc(p, 12);
        *(int*) ( ref_lock(&h) ) = idx++;
        ref_unlock(&h);
    }

    // Iterate each object
    idx = 0;
    refpool_foreach(p, &idx, [] (void* i, auto h) {
        REQUIRE(*(int*) ref_lock(h) == ( *(int*) i )++);
        ref_unlock(h);
    });

    SECTION("Free All")
    {
        for ( auto& h : hd )
        {
            ref_free(&h);
        }
    }
    SECTION("Lock before Free")
    {
        for ( auto& h : hd )
            ref_lock(&h);
        for ( auto& h : hd )
            ref_free(&h);
        for ( auto& h : hd )
            ref_unlock(&h);
    }

    REQUIRE(refpool_num_available(p));
}