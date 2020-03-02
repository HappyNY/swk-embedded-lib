#include <Catch2/catch.hpp>
#include <iostream>
#include <uEmbedded-pp/math/matrix_static.hpp>
#include <uEmbedded-pp/math/matrix_utility.hpp>

template <typename v, size_t a, size_t b>
struct dd
{
    v data__[a][b];
};

template <typename va, typename vb>
struct arithmetic_result
{
    auto operator()() { return va() * vb(); }
};

template <typename va, typename vb>
using arithmetic_result_t
  = std::invoke_result_t<arithmetic_result<va, vb>( void )>;

template <typename va, typename vb, size_t a, size_t b, size_t c>
static dd<arithmetic_result_t<va, vb>, a, c>
multiply( dd<va, a, b> g1, dd<vb, b, c> g2 )
{
    return dd<arithmetic_result_t<va, vb>, a, c>();
}

TEST_CASE( "Matrix basic functionalities", "[math-matrix]" )
{
    dd<int, 3, 4>   c;
    dd<float, 4, 5> f;
    multiply( c, f );
}

TEST_CASE( "Matrix basic functionalities", "[math-matrix]" )
{
    using namespace upp::math;
    using namespace std;

    static_matrix<int, 3, 3> ff = {};
    static_matrix<int, 3, 3> gg = {};

    ff( 0, 0 ) = 3;
    ff( 1, 1 ) = 3;
    ff( 2, 2 ) = 4;
    gg( 1, 1 ) = 6;
    gg( 2, 2 ) = 7;
    gg( 0, 0 ) = 7;
}

void nfunc( int a, int b )
{
    using namespace upp::math;
    using namespace std;

    static_matrix<int, 4, 5> gg;
    static_matrix<int, 5, 4> kk;
     
}
