#include <Catch2/catch.hpp>
#include <iostream>
#include <uEmbedded-pp/math/matrix_static.hpp>
#include <uEmbedded-pp/math/matrix_utility.hpp>

TEST_CASE( "Matrix basic functionalities", "[math-matrix]" )
{
    using namespace upp::math;

    static_matrix<int, 4, 3> ff = {}, gg = {};

    ff( 1 ) = 3;
    ff( 2 ) = 4;
    gg( 1 ) = 6;
    gg( 3 ) = 7;

    auto cc = ff + gg;

    std::cout << std::to_string( cc.desc() ) << std::endl;
}
