#include <Catch2/catch.hpp>
extern "C" {
#include <uEmbedded/timer_logic.h>
}

#include <uEmbedded-pp/static_timer_logic.hxx>

TEST_CASE( "Timer logic functionality test", "[timer-logic]" )
{
    timer_logic s;
    enum { CAP = 0x1000 };
    auto           cnt = timer_init( &s, malloc( CAP ), CAP );
    timer_handle_t h[20];

    double v = 0.0f;

    h[0] = timer_add(
        &s, 2000, []( void* o ) { *(double*)o = 2.0; }, &v );
    h[1] = timer_add(
        &s, 1000, []( void* o ) { *(double*)o = 1.0; }, &v );
    auto x = h[2] = timer_add(
        &s, 3000, []( void* o ) { *(double*)o = 3.0; }, &v );

    REQUIRE( timer_nextTrigger( &s ) == 1000 );
    REQUIRE( timer_browse( &s, x )->triggerTime == 3000 );
    REQUIRE( timer_browse( &s, x )->timerId == 2 );

    for ( size_t i = 0, length = 10; i < length; i++ ) {
        h[3 + i] = timer_add(
            &s, 4000 + i * 1000, []( void* o ) { *(double*)o += 1.0; }, &v );
    }

    SECTION( "Update timers" )
    {
        for ( size_t i = 0; i < 14; ++i ) {
            timer_update( &s, i * 1000 );
            REQUIRE( v == i );
        }
    }
    SECTION( "Update timers after remove some " )
    {
        timer_erase( &s, h[4] );
        timer_erase( &s, h[5] );
        timer_erase( &s, h[6] );

        timer_update( &s, 100000 );
        REQUIRE( v == 10.0f );
    }
}

TEST_CASE( "Timer logic cpp version test", "[timer-logic]" )
{
    upp::timer_logic<uint64_t, uint8_t, 128> tim;
    decltype( tim )::tick_type               ticks = 0;
    std::vector<int>                         tester;

    tim.tick_function( [&ticks]() { return ticks; } );

    tim.add( 1000, &tester, []( void* o ) { ( (std::vector<int>*)o )->push_back( 3 ); } );
    tim.add( 2000, &tester, []( void* o ) { ( (std::vector<int>*)o )->push_back( 3 ); } );
    auto rmv = tim.add( 3000, &tester, []( void* o ) { ( (std::vector<int>*)o )->push_back( 3 ); } );
    tim.add( 5000, &tester, []( void* o ) { ( (std::vector<int>*)o )->push_back( 3 ); } );
    tim.add( 6000, &tester, []( void* o ) { ( (std::vector<int>*)o )->push_back( 3 ); } );

    REQUIRE( tim.size() == 5 );
    REQUIRE( tester.size() == 0 );
    tim.update();
    REQUIRE( tester.size() == 0 );

    ticks = 1000;
    tim.update();
    REQUIRE( tester.size() == 1 );

    tim.update();
    REQUIRE( tim.size() == 4 );
    REQUIRE( tester.size() == 1 );

    REQUIRE( tim.remove( rmv ) );
    REQUIRE( tim.size() == 3 );

    ticks = 3001;
    tim.update();
    REQUIRE( tester.size() == 2 );
    REQUIRE( tim.size() == 2 );
    
    ticks = 6000;
    tim.update();
    REQUIRE( tester.size() == 4 );
    REQUIRE( tim.size() == 0 );
}