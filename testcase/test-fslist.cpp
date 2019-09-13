extern "C"
{
#include "swk-embedded-c/fslist.h"
}
#include "catch2/catch.hpp"
#include <vector>
#include <cstdlib>
#include <tuple>

TEST_CASE( "fslist features works correctly.", "[fslist]" )
{
    fslist list;
    
    enum {
        CAP = 10000
    };
    fslistInit( &list, CAP, sizeof( double ) );

    INFO( "Testing parameter setups." );
    REQUIRE( list.size == 0 );
    REQUIRE( list.availableHead == 0 );
    REQUIRE( list.dataPool != NULL );
    REQUIRE( list.pool != NULL );
    REQUIRE( list.elemSize == sizeof( double ) );
    REQUIRE( list.poolCapacity == CAP );
     
    INFO( "Testing linkage." );
    for ( size_t i = 0; i < CAP-1; i++ )
    {
        REQUIRE( list.pool[i].next == i+1 );
        REQUIRE( list.pool[i + 1].prev == i );
        REQUIRE( list.pool[i].pdata == NULL );
    }
    REQUIRE( list.pool[0].prev == FSLIST_NODEIDX_NONE );
    REQUIRE( list.pool[CAP - 1].next == FSLIST_NODEIDX_NONE );
    REQUIRE( list.pool[CAP - 1].pdata == NULL ); 

    // Allocate few nodes
    std::vector<std::pair<double, fslistNodeIdx_t>> pairs;
    for ( size_t i = 0, length = rand() % CAP; i < length; i++ )
    {
        double v = rand();

        auto idx = fslistGetOffset( &list, fslistInsertNew( &list, FSLIST_NODEIDX_NONE, &v ));
        REQUIRE( idx != FSLIST_NODEIDX_NONE );
        REQUIRE( idx == i );

        pairs.emplace_back( std::make_pair( v, idx ) );
    }
    
    INFO( "Testing allocated counts." );
    REQUIRE( pairs.size() == list.size );

    INFO( "Allocated data is correct." );
    for ( auto& p : pairs )
    {
        REQUIRE( *( double*) list.pool[p.second].pdata == p.first );
    }

    SECTION( "Deallocated all nodes randomly" )
    {
        int numToRelease = rand() % pairs.size();
        while ( numToRelease )
        {
            int idx = rand() % numToRelease--;
            REQUIRE( fslistErase( &list, pairs[idx].second ) );
            pairs.erase( pairs.begin() + idx );
            REQUIRE( list.size == pairs.size() );
        }
    }

    SECTION( "Destructors are called correctly." )
    {
        static int dtorcnt = 0;
        auto dtor = [] ( void* ) { dtorcnt++; };
        list.dtorCallback = dtor;
        
        REQUIRE( fslistDestroy( &list ) );
        REQUIRE( dtorcnt == pairs.size() );
    }
}