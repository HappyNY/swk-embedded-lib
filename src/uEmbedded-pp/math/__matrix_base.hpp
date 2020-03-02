#pragma once
#include <type_traits>
#include "../uassert.h"

namespace upp { namespace math { namespace matrix_impl {
//! @addtogroup uEmbedded_Cpp
//! @{
//! @addtogroup uEmedded_Cpp_Math
//! @{
//! @addtogroup uEmedded_Cpp_Math_Matrix
//! @{
//! @addtogroup uEmedded_Cpp_Math_Matrix_Impl
//! @brief      Set of basic functionalities for handling matrix.
//! @{

//! @brief      Matrix descriptor.
//! @details
//!              Supports only 1 channel matrix in default. However, any
//!             compatible type can be used as a template argument of a matrix,
//!             you can create your own multi-channel matrix by using various
//!             value types.
//! @{
template <typename data_t__>
struct cdesc
{
    size_t          row__;
    size_t          col__;
    data_t__ const* dptr__;

    using data_type = data_t__;
};

template <typename data_t__>
struct desc
{
    size_t    row__;
    size_t    col__;
    data_t__* dptr__;

    using data_type = data_t__;

    operator cdesc<data_type>() const noexcept
    {
        cdesc<data_type> ret;
        ret.row__  = row__;
        ret.col__  = col__;
        ret.dptr__ = dptr__;
        return ret;
    }
};
//! @}

//! @brief      For result deduction ...
//! @{
template <typename t_0__, typename t_1__>
struct arithmatic_result
{
    auto operator()() { return t_0__() + t_1__(); }
};

#ifdef __INTELLISENSE__
template <typename t_0__, typename t_1__>
using arith_result_t
  = std::invoke_result_t<arithmatic_result<t_0__, t_1__>( void )>;
#else
template <typename t_0__, typename t_1__>
using arith_result_t = std::invoke_result_t<arithmatic_result<t_0__, t_1__>>;
#endif

template <typename t_0__, typename t_1__>
using add_result_t = arith_result_t<t_0__, t_1__>;
template <typename t_0__, typename t_1__>
using subtract_result_t = arith_result_t<t_0__, t_1__>;
template <typename t_0__, typename t_1__>
using multiply_result_t = arith_result_t<t_0__, t_1__>;
template <typename t_0__, typename t_1__>
using divide_result_t = arith_result_t<t_0__, t_1__>;
//! @}

//! @brief      Basic functionalities for handling matrix
//! @{
#define UPP_MATH_MATRIX_COMMON_BEGIN()                                         \
    using data_t = std::remove_reference_t<decltype( o )>::data_type;          \
    uassert( l.row__ == r.row__ && r.row__ == o.row__ );                       \
    uassert( l.col__ == r.col__ && r.col__ == o.col__ );                       \
    uassert( capacity_out == o.col__ * o.row__ * sizeof( data_t ) );           \
                                                                               \
    auto rows = o.row__;                                                       \
    auto cols = o.col__;                                                       \
                                                                               \
    for ( size_t i = 0; i < rows; i++ ) {                                      \
        auto lptr = l.dptr__ + i * cols;                                       \
        auto rptr = r.dptr__ + i * cols;                                       \
        auto optr = o.dptr__ + i * cols;                                       \
        for ( size_t j = 0; j < cols; j++ ) {
#define UPP_MATH_MATRIX_COMMON_END()                                           \
    }                                                                          \
    }

#define UPP_MATH_MATRIX_COMMON_SCALAR_BEGIN()                                  \
    using data_t = std::remove_reference_t<decltype( o )>::data_type;          \
    uassert( l.row__ == o.row__ && l.col__ == o.col__ );                       \
    uassert( capacity_out == o.col__ * o.row__ * sizeof( data_t ) );           \
    auto rows = o.col__;                                                       \
    auto cols = o.row__;                                                       \
                                                                               \
    for ( size_t i = 0; i < rows; i++ ) {                                      \
        auto lptr = l.dptr__ + i * cols;                                       \
        auto optr = o.dptr__ + i * cols;                                       \
        for ( size_t j = 0; j < cols; j++ ) {
#define UPP_MATH_MATRIX_COMMON_SCALAR_END()                                    \
    }                                                                          \
    }

#define UPP_MATRIX_COMMON_SINGULAR_BEGIN()                                     \
    using data_t = std::remove_reference_t<decltype( o )>::data_type;          \
    uassert( l.row__ == o.col__ && l.col__ == o.row__ );                       \
    uassert( capacity_out == o.col__ * o.row__ * sizeof( data_t ) );           \
                                                                               \
    auto rows = o.row__;                                                       \
    auto cols = o.col__;                                                       \
                                                                               \
    for ( size_t i = 0; i < rows; i++ ) {                                      \
        auto lptr = l.dptr__ + i * cols;                                       \
        auto optr = o.dptr__ + i * cols;                                       \
        for ( size_t j = 0; j < cols; j++ ) {
#define UPP_MATRIX_COMMON_SINGULAR_END()                                       \
    }                                                                          \
    }

template <typename t_0__, typename t_1__>
void add(
  cdesc<t_0__> const&                     l,
  cdesc<t_1__> const&                     r,
  desc<add_result_t<t_0__, t_1__>> const& o,
  size_t                                  capacity_out ) noexcept
{
    UPP_MATH_MATRIX_COMMON_BEGIN();
    optr[j] = lptr[j] + rptr[j];
    UPP_MATH_MATRIX_COMMON_END();
}

template <typename t_0__, typename t_1__>
void subtract(
  cdesc<t_0__> const&                          l,
  cdesc<t_1__> const&                          r,
  desc<subtract_result_t<t_0__, t_1__>> const& o,
  size_t                                       capacity_out ) noexcept
{
    UPP_MATH_MATRIX_COMMON_BEGIN();
    optr[j] = lptr[j] - rptr[j];
    UPP_MATH_MATRIX_COMMON_END();
}

template <typename t_0__, typename t_1__>
void multiply(
  cdesc<t_0__> const&                          l,
  cdesc<t_1__> const&                          r,
  desc<multiply_result_t<t_0__, t_1__>> const& o,
  size_t                                       capacity_out ) noexcept
{
    // m x n * n x l = m x l
    using data_t = std::remove_reference_t<decltype( o )>::data_type;

    uassert( o.row__ == l.row__ && o.col__ == r.col__ && l.col__ == r.row__ );
    uassert( capacity_out == o.col__ * o.row__ * sizeof( data_t ) );
    auto rows  = o.col__;
    auto cols  = o.row__;
    auto n     = l.col__;
    auto rofst = r.col__;

    for ( size_t i = 0; i < rows; i++ ) {
        auto optr = o.dptr__ + i * cols;
        auto lb   = l.dptr__ + i * l.col__;
        for ( size_t j = 0; j < cols; j++ ) {
            // Apply dot product of l(i, :) and r(:, j)
            data_t acc = {};
            auto   rb  = r.dptr__ + j;
            for ( size_t k = 0; k < n; k++ ) {
                acc += lb[k] * rb[0];
                rb += rofst;
            }
            optr[j] = acc;
        }
    }
}

template <typename t_0__, typename t_1__>
void elem_multiply(
  cdesc<t_0__> const&                          l,
  cdesc<t_1__> const&                          r,
  desc<multiply_result_t<t_0__, t_1__>> const& o,
  size_t                                       capacity_out ) noexcept
{
    UPP_MATH_MATRIX_COMMON_BEGIN();
    optr[j] = lptr[j] * rptr[j];
    UPP_MATH_MATRIX_COMMON_END();
}

template <typename t_0__, typename t_1__>
void elem_divide(
  cdesc<t_0__> const&                        l,
  cdesc<t_1__> const&                        r,
  desc<divide_result_t<t_0__, t_1__>> const& o,
  size_t                                     capacity_out ) noexcept
{
    UPP_MATH_MATRIX_COMMON_BEGIN();
    optr[j] = lptr[j] / rptr[j];
    UPP_MATH_MATRIX_COMMON_END();
}

template <typename t_0__, typename t_1__>
void scalar_multiply(
  cdesc<t_0__> const&                          l,
  t_1__ const&                                 r,
  desc<multiply_result_t<t_0__, t_1__>> const& o,
  size_t                                       capacity_out ) noexcept
{
    UPP_MATH_MATRIX_COMMON_SCALAR_BEGIN();
    optr[j] = lptr[j] * r;
    UPP_MATH_MATRIX_COMMON_SCALAR_END();
}

template <typename t_0__, typename t_1__>
void scalar_divide_by_mat(
  cdesc<t_0__> const&                          l,
  t_1__ const&                                 r,
  desc<multiply_result_t<t_0__, t_1__>> const& o,
  size_t                                       capacity_out ) noexcept
{
    UPP_MATH_MATRIX_COMMON_SCALAR_BEGIN();
    optr[j] = r / lptr[j];
    UPP_MATH_MATRIX_COMMON_SCALAR_END();
}

template <typename t_0__, typename t_1__>
void scalar_divide(
  cdesc<t_0__> const&                          l,
  t_1__ const&                                 r,
  desc<multiply_result_t<t_0__, t_1__>> const& o,
  size_t                                       capacity_out ) noexcept
{
    UPP_MATH_MATRIX_COMMON_SCALAR_BEGIN();
    optr[j] = lptr[j] / r;
    UPP_MATH_MATRIX_COMMON_SCALAR_END();
}

template <typename t_0__, typename t_1__>
void scalar_add(
  cdesc<t_0__> const&                          l,
  t_1__ const&                                 r,
  desc<multiply_result_t<t_0__, t_1__>> const& o,
  size_t                                       capacity_out ) noexcept
{
    UPP_MATH_MATRIX_COMMON_SCALAR_BEGIN();
    optr[j] = lptr[j] + r;
    UPP_MATH_MATRIX_COMMON_SCALAR_END();
}

template <typename t_0__>
void negate(
  cdesc<t_0__> const& l,
  desc<t_0__>&        o,
  size_t              capacity_out ) noexcept
{
    UPP_MATRIX_COMMON_SINGULAR_BEGIN();
    optr[j] = -lptr[j];
    UPP_MATRIX_COMMON_SINGULAR_END();
}

template <typename t_0__, typename t_1__>
void substitute(
  cdesc<t_0__> const& l,
  desc<t_1__>&        o,
  size_t              capacity_out ) noexcept
{
    UPP_MATRIX_COMMON_SINGULAR_BEGIN();
    optr[j] = lptr[j];
    UPP_MATRIX_COMMON_SINGULAR_END();
}

template <typename t_0__, typename t_1__>
void scalar_substitute(
  t_0__              r,
  desc<t_1__> const& o,
  size_t             capacity_out ) noexcept
{
    using data_t = std::remove_reference_t<decltype( o )>::data_type;
    uassert( capacity_out == o.col__ * o.row__ * sizeof( data_t ) );

    auto rows = o.col__;
    auto cols = o.row__;

    for ( size_t i = 0; i < rows; i++ ) {
        auto optr = o.dptr__ + i * cols;
        for ( size_t j = 0; j < cols; j++ ) {
            optr[j] = static_cast<data_t>( r );
        }
    }
}

template <typename t_0__>
void transpose(
  cdesc<t_0__> const& l,
  desc<t_0__>&        o,
  size_t              capacity_out ) noexcept
{
    UPP_MATRIX_COMMON_SINGULAR_BEGIN();
    optr[j] = l.dptr__[j * l.col__ + i];
    UPP_MATRIX_COMMON_SINGULAR_END();
}

//! @}
//! @brief      Logical operations
//! @{
template <typename t_0__, typename t_1__>
void equals(
  cdesc<t_0__> const& l,
  cdesc<t_1__> const& r,
  desc<bool>&         o,
  size_t              capacity_out ) noexcept
{
    UPP_MATH_MATRIX_COMMON_BEGIN();
    optr[j] = lptr[j] == rptr[j];
    UPP_MATH_MATRIX_COMMON_END();
}

template <typename t_0__, typename t_1__>
void not_equals(
  cdesc<t_0__> const& l,
  cdesc<t_1__> const& r,
  desc<bool>&         o,
  size_t              capacity_out ) noexcept
{
    UPP_MATH_MATRIX_COMMON_BEGIN();
    optr[j] = lptr[j] != rptr[j];
    UPP_MATH_MATRIX_COMMON_END();
}

template <typename t_0__, typename t_1__>
void less_than(
  cdesc<t_0__> const& l,
  cdesc<t_1__> const& r,
  desc<bool>&         o,
  size_t              capacity_out ) noexcept
{
    UPP_MATH_MATRIX_COMMON_BEGIN();
    optr[j] = lptr[j] < rptr[j];
    UPP_MATH_MATRIX_COMMON_END();
}

template <typename t_0__, typename t_1__>
void greater_than(
  cdesc<t_0__> const& l,
  cdesc<t_1__> const& r,
  desc<bool>&         o,
  size_t              capacity_out ) noexcept
{
    UPP_MATH_MATRIX_COMMON_BEGIN();
    optr[j] = lptr[j] > rptr[j];
    UPP_MATH_MATRIX_COMMON_END();
}

template <typename t_0__, typename t_1__>
void less_or_equal(
  cdesc<t_0__> const& l,
  cdesc<t_1__> const& r,
  desc<bool>&         o,
  size_t              capacity_out ) noexcept
{
    UPP_MATH_MATRIX_COMMON_BEGIN();
    optr[j] = lptr[j] <= rptr[j];
    UPP_MATH_MATRIX_COMMON_END();
}

template <typename t_0__, typename t_1__>
void greater_or_equal(
  cdesc<t_0__> const& l,
  cdesc<t_1__> const& r,
  desc<bool>&         o,
  size_t              capacity_out ) noexcept
{
    UPP_MATH_MATRIX_COMMON_BEGIN();
    optr[j] = lptr[j] >= rptr[j];
    UPP_MATH_MATRIX_COMMON_END();
}

template <typename t_0__, typename t_1__>
void logic_and(
  cdesc<t_0__> const& l,
  cdesc<t_1__> const& r,
  desc<bool>&         o,
  size_t              capacity_out ) noexcept
{
    UPP_MATH_MATRIX_COMMON_BEGIN();
    optr[j] = lptr[j] && rptr[j];
    UPP_MATH_MATRIX_COMMON_END();
}

template <typename t_0__, typename t_1__>
void logic_or(
  cdesc<t_0__> const& l,
  cdesc<t_1__> const& r,
  desc<bool>&         o,
  size_t              capacity_out ) noexcept
{
    UPP_MATH_MATRIX_COMMON_BEGIN();
    optr[j] = lptr[j] || rptr[j];
    UPP_MATH_MATRIX_COMMON_END();
}

template <typename t_0__>
void not(
  cdesc<t_0__> const& l,
  desc<bool> const&   o,
  size_t              capacity_out ) noexcept
{
    UPP_MATRIX_COMMON_SINGULAR_BEGIN();
    optr[j] = !lptr[j];
    UPP_MATRIX_COMMON_SINGULAR_END();
}

template <typename t_0__, typename t_1__>
void scalar_equals(
  cdesc<t_0__> const& l,
  t_1__ const&        r,
  desc<bool>&         o,
  size_t              capacity_out ) noexcept
{
    UPP_MATH_MATRIX_COMMON_SCALAR_BEGIN();
    optr[j] = lptr[j] == r;
    UPP_MATH_MATRIX_COMMON_SCALAR_END();
}

template <typename t_0__, typename t_1__>
void scalar_neq(
  cdesc<t_0__> const& l,
  t_1__ const&        r,
  desc<bool>&         o,
  size_t              capacity_out ) noexcept
{
    UPP_MATH_MATRIX_COMMON_SCALAR_BEGIN();
    optr[j] = lptr[j] != r;
    UPP_MATH_MATRIX_COMMON_SCALAR_END();
}

template <typename t_0__, typename t_1__>
void scalar_lt(
  cdesc<t_0__> const& l,
  t_1__ const&        r,
  desc<bool>&         o,
  size_t              capacity_out ) noexcept
{
    UPP_MATH_MATRIX_COMMON_SCALAR_BEGIN();
    optr[j] = lptr[j] < r;
    UPP_MATH_MATRIX_COMMON_SCALAR_END();
}

template <typename t_0__, typename t_1__>
void scalar_gt(
  cdesc<t_0__> const& l,
  t_1__ const&        r,
  desc<bool>&         o,
  size_t              capacity_out ) noexcept
{
    UPP_MATH_MATRIX_COMMON_SCALAR_BEGIN();
    optr[j] = lptr[j] >= r;
    UPP_MATH_MATRIX_COMMON_SCALAR_END();
}

template <typename t_0__, typename t_1__>
void scalar_lteq(
  cdesc<t_0__> const& l,
  t_1__ const&        r,
  desc<bool>&         o,
  size_t              capacity_out ) noexcept
{
    UPP_MATH_MATRIX_COMMON_SCALAR_BEGIN();
    optr[j] = lptr[j] <= r;
    UPP_MATH_MATRIX_COMMON_SCALAR_END();
}

template <typename t_0__, typename t_1__>
void scalar_gteq(
  cdesc<t_0__> const& l,
  t_1__ const&        r,
  desc<bool>&         o,
  size_t              capacity_out ) noexcept
{
    UPP_MATH_MATRIX_COMMON_SCALAR_BEGIN();
    optr[j] = lptr[j] >= r;
    UPP_MATH_MATRIX_COMMON_SCALAR_END();
}

//! @}

//! @}
//! @}
//! @}
//! @}
}}} // namespace upp::math::matrix_impl