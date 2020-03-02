#include <Catch2/catch.hpp>
#include <iostream>
#include <uEmbedded-pp/math/matrix_static.hpp>
#include <uEmbedded-pp/math/matrix_utility.hpp>

TEST_CASE( "Matrix basic functionalities", "[math-matrix]" )
{
    using namespace upp::math;
    using namespace std;

    static_matrix<int, 3, 3> ff = {}, gg = {};

    ff( 0, 0 ) = 3;
    ff( 1, 1 ) = 3;
    ff( 2, 2 ) = 4;
    gg( 1, 1 ) = 6;
    gg( 2, 2 ) = 7;
    gg( 0, 0 ) = 7;

    cout << to_string( ( ff * gg ).cdesc() ) << endl;
}
