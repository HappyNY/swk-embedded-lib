#include <Catch2/catch.hpp>
#include <iostream>
#include <uEmbedded-pp/math/matrix_static.hpp>
#include <uEmbedded-pp/math/matrix_stream_utility.hpp>

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
    using namespace upp::math;
    using namespace std;

    static_matrix<float, 3, 3>  ff = {};
    static_matrix<double, 3, 3> gg = {};

    for ( auto& d : ff ) {
        d = rand() / (double)RAND_MAX;
    }
    cout << ff << gg << endl << ff * ff << endl;
}