#include <Catch2/catch.hpp>
#include <algorithm>
#include <list>

extern "C" {
#include <uEmbedded/fslist.h>
}
#include <uEmbedded-pp/static_fslist.hxx>
int v;

TEST_CASE( "fslist functionalities.", "[fslist]" )
{
    fslist v;
    size_t constexpr CAP = 32768;
    size_t cnt = fslist_init( &v, malloc( CAP ), CAP, sizeof( double ) );

    REQUIRE( v.elemSize == sizeof( double ) );

    SECTION( "On adding first argument" )
    {
        auto n = fslist_insert( &v, NULL );
        REQUIRE( fslist_idx( &v, n ) == v.tail );
        REQUIRE( fslist_idx( &v, n ) == v.head );
        REQUIRE( v.size == 1 );
        REQUIRE( n->isValid );
        REQUIRE( n->next == FSLIST_NODEIDX_NONE );
        REQUIRE( n->prev == FSLIST_NODEIDX_NONE );

        SECTION( "Adding elements from head" )
        {
            for ( size_t i = 1; i < cnt; i++ ) {
                auto n = fslist_insert( &v, v.get + v.head );
                REQUIRE( fslist_idx( &v, n ) == v.head );
                REQUIRE( v.size == i + 1 );
            }
        }
        SECTION( "Inserting elements on tail" )
        {
            for ( size_t i = 1; i < cnt; i++ ) {
                auto tail = v.tail;
                auto n    = fslist_insert( &v, v.get + v.tail );
                REQUIRE( v.tail == tail );
                REQUIRE( v.size == i + 1 );
            }
        }
    }

    SECTION( "On list fullfilled" )
    {
        for ( size_t i = 0; i < cnt; i++ ) {
            auto n = fslist_insert( &v, NULL );
            REQUIRE( fslist_idx( &v, n ) == v.tail );
            REQUIRE( v.size == i + 1 );
        }

        REQUIRE( v.inactive == FSLIST_NODEIDX_NONE );

        SECTION( "On emptying list" )
        {
            while ( v.size ) {
                auto idx = fslist_idx( &v, &v.get[v.head] );
                fslist_erase( &v, v.get + v.head );
                REQUIRE( v.inactive == idx );
            }
        }
    }

    free( v.buff );
}

TEST_CASE( "fslist Cplusplus version", "[fslist]" )
{
    constexpr size_t num_case = 55;

    upp::static_fslist<double, size_t, num_case> f;

    for ( size_t i = 0; i < num_case; i++ ) {
        f.emplace_back( rand() );
    }

    REQUIRE( f.size() == num_case );
    REQUIRE( f.max_size() - f.size() == 0 );

    for ( size_t i = 0; i < num_case / 2; i++ ) {
        f.pop_back();
    }

    REQUIRE( f.size() == num_case - num_case / 2 );

    for ( size_t i = 0; i < num_case / 2; i++ ) {
        f.emplace_front( rand() );
    }

    REQUIRE( f.max_size() - f.size() == 0 );

    f.clear();
    REQUIRE( f.size() == 0 );

    std::vector<double> v;

    for ( size_t i = 0; i < num_case; i++ ) {
        auto val = double( rand() );

        v.push_back( val );
        f.push_back( val );
    }

    REQUIRE( std::equal( v.begin(), v.end(), f.begin(), f.end() ) );
}