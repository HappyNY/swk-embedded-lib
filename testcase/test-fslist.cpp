#include <Catch2/catch.hpp>

extern "C" {
#include <fslist.h>
}

TEST_CASE( "fslist functionalities.", "[fslist]" )
{
    fslist v;
    size_t constexpr CAP = 32768;
    size_t cnt = fslist_init( &v, malloc( CAP ), CAP , sizeof( double ) );

    REQUIRE( cnt == CAP/ ( sizeof( fslist_node ) + sizeof( double ) ) );
    REQUIRE( v.elemSize == sizeof( double ) );

    SECTION( "On adding first argument" )
    {
        auto n = fslist_insert( &v, NULL );
        REQUIRE( fslist_idx( &v, n ) == v.tail );
        REQUIRE( fslist_idx( &v, n ) == v.head );
        REQUIRE( v.size == 1 );
        REQUIRE( n->object );
        REQUIRE( n->next == FSLIST_NODEIDX_NONE );
        REQUIRE( n->prev == FSLIST_NODEIDX_NONE );

        SECTION( "Adding elements from head" )
        {
            for ( size_t i = 1; i < cnt; i++ )
            {
                auto n = fslist_insert( &v, v.get + v.head );
                REQUIRE( fslist_idx( &v, n ) == v.head );
                REQUIRE( v.size == i + 1 );
            }
        }
        SECTION( "Inserting elements on tail" )
        {
            for ( size_t i = 1; i < cnt; i++ )
            {
                auto tail = v.tail;
                auto n = fslist_insert( &v, v.get + v.tail );
                REQUIRE( v.tail == tail );
                REQUIRE( v.size == i + 1 );
            }
        }
    }

    SECTION( "On list fullfilled" )
    {
        for ( size_t i = 0; i < cnt; i++ )
        {
            auto n = fslist_insert( &v, NULL );
            REQUIRE( fslist_idx( &v, n ) == v.tail );
            REQUIRE( v.size == i + 1 );
        }

        REQUIRE( v.inactive == FSLIST_NODEIDX_NONE );

        SECTION( "On emptying list" )
        {
            while ( v.size )
            {
                auto idx = fslist_idx( &v, &v.get[v.head] );
                fslist_erase( &v, v.get + v.head );
                REQUIRE( v.inactive == idx );
            }
        }
    }

    free( v.buff );
}