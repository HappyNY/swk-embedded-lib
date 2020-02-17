#include <Catch2/catch.hpp>
#include <random>
#include <uEmbedded-pp/utility.hxx>
#include <vector>
extern "C" {
#include "uEmbedded/algorithm.h"
}

using namespace std;

TEST_CASE( "Test lowerbound", "[algorithm]" )
{
    vector<double>              v;
    uniform_real_distribution<> db;
    random_device               rd;
    mt19937_64                  mt( rd() );

    enum
    {
        numElem = 0x10000
    };
    v.reserve( numElem );

    for ( size_t i = 0; i < numElem; i++ )
    {
        double val = db( mt );
        auto   idx = lowerbound(
          v.data(),
          &val,
          sizeof( double ),
          v.size(),
          []( auto a, auto b ) -> int {
              auto v = *(double*)a - *(double*)b;
              return v < 0 ? -1 : v > 0 ? 1 : 0;
          } );
        REQUIRE( idx <= v.size() );
        v.insert( v.begin() + idx, val );
    }

    // for ( int i = 0; i < numElem; ++i )
    // {
    //     v.push_back(db(mt));
    // }

    sort( v.begin(), v.end() );

    for ( int i = 0; i < numElem; ++i )
    {
        double val = db( mt );
        auto   org = std::lower_bound( v.begin(), v.end(), val ) - v.begin();
        auto   idx = lowerbound(
          v.data(),
          &val,
          sizeof( double ),
          numElem,
          []( auto a, auto b ) -> int {
              auto v = *(double*)a - *(double*)b;
              return v < 0 ? -1 : v > 0 ? 1 : 0;
          } );

        REQUIRE( org == idx );
    }
}

TEST_CASE( "Test btoa/atob", "[algorithm]" )
{
    struct big_mass_t
    {
        int arguments[64];
    } mass;

    for ( size_t i = 0;
          i < sizeof( mass.arguments ) / sizeof( *mass.arguments );
          i++ )
    {
        auto v            = ( rand() / (float)RAND_MAX );
        mass.arguments[i] = *(int*)&v;
    }

    char cp[sizeof( mass ) * 2];
    upp::binutil::btoa( cp, sizeof mass * 2, &mass, sizeof mass );
    big_mass_t clone;
    upp::binutil::atob( cp, &clone, sizeof clone );

    cp[16] = 0;
    INFO( "STRING :: " << std::string( cp ) );
    sprintf( cp, "%08x %08x", mass.arguments[0], mass.arguments[1] );
    INFO( "SOURCE :: " << cp );
    sprintf( cp, "%08x %08x", clone.arguments[0], clone.arguments[1] );
    INFO( "CLONE  :: " << cp );
    REQUIRE( memcmp( &clone, &mass, sizeof mass ) == 0 );
}