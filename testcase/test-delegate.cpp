#include <Catch2/catch.hpp>
extern "C"
{
#include <uEmbedded/delegate.h>
}    

static delegate_t* evnt = delegate_create(true);

static void increment(refhandle const* h, void* v)
{
    int& i = *(int*) v;
    if ( i++ & 1 )
    {
        delegate_delete(evnt, h, increment);
    }
}

TEST_CASE("delegate test", "[delegate_pool]")
{
    return;

    managed_reference_pool_t* p;
    p = refpool_create(100);
    refhandle harr[10];
    for ( auto& h : harr )
        h = refpool_malloc(p, sizeof(int));  

    for ( auto& h : harr )
        delegate_assign(evnt, &h, increment);
    
    REQUIRE(delegate_size(evnt) == _countof(harr));

    int v = 0;
    delegate_call(evnt, &v);

    REQUIRE(v == 10);
    
    REQUIRE(delegate_size(evnt) == 5);
    delegate_call(evnt, &v);

    REQUIRE(v == 15);

    delegate_destroy(evnt);
}