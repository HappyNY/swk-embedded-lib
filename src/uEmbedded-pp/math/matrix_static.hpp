#pragma once
#include <cstdlib>
#include "__matrix_base.hpp"

namespace upp { namespace math {
//! @addtogroup uEmbedded_Cpp
//! @{
//! @addtogroup uEmedded_Cpp_Math
//! @{
//! @addtogroup uEmedded_Cpp_Math_Matrix
//! @{

//! @brief      Supports for easy-to-use arithmetic operations
template <typename vty__, size_t row__, size_t col__>
class static_matrix
{
public:
    enum : size_t
    {
        num_rows = row__,
        num_cols = col__,
        mat_size = num_rows * num_cols * sizeof( vty__ )
    };
    using value_type = vty__;
    using cdesc_type = matrix_impl::cdesc<value_type>;
    using desc_type  = matrix_impl::desc<value_type>;

    using iterator       = value_type*;
    using const_iterator = value_type const*;

public:
    desc_type desc() noexcept
    {
        desc_type ret;
        ret.col__  = num_cols;
        ret.row__  = num_rows;
        ret.dptr__ = data;
        return ret;
    }

    cdesc_type cdesc() const noexcept
    {
        cdesc_type ret;
        ret.col__  = num_cols;
        ret.row__  = num_rows;
        ret.dptr__ = data;
        return ret;
    }

    iterator       begin() noexcept { return data; }
    iterator       end() noexcept { return *( &data + 1 ); }
    const_iterator cbegin() const noexcept { return data; }
    const_iterator cend() const noexcept { return *( &data + 1 ); }

public:
    static_matrix() = default;

    template <typename ty__>
    static_matrix&
    operator=( const static_matrix<ty__, num_rows, num_cols>& right )
    {
        matrix_impl::substitute( right.cdesc(), desc(), mat_size );
        return *this;
    }

    template <typename ty__>
    static_matrix( const static_matrix<ty__, num_rows, num_cols>& right )
    {
        *this = right;
    }

    template <typename ty__>
    static_matrix& operator=( const ty__& value )
    {
        matrix_impl::scalar_substitute( value, desc(), mat_size );
        return *this;
    }

    template <typename ty__>
    static_matrix( const ty__& value )
    {
        *this = value;
    }

public:
    auto& operator()( size_t row, size_t col ) const noexcept
    {
        uassert( row < num_rows && col < num_cols );
        return data[row * num_cols + col];
    }
    auto& operator()( size_t row, size_t col ) noexcept
    {
        uassert( row < num_rows && col < num_cols );
        return data[row * num_cols + col];
    }

    auto& operator()( size_t idx ) const noexcept { return data[idx]; }
    auto& operator()( size_t idx ) noexcept { return data[idx]; }

    value_type const* ptr() const noexcept { return data; }
    value_type*       ptr() noexcept { return data; }

    auto row( size_t row ) const noexcept
    {
        uassert( row < num_rows );
        static_matrix<value_type, 1, num_cols> ret;
        memcpy( ret.data, data + row * num_cols, ret.mat_size );
        return ret;
    }

    auto col( size_t col ) const noexcept
    {
        uassert( col < num_cols );
        static_matrix<value_type, num_rows, 1> ret;

        auto begin = data + col;
        for ( size_t i = 0; i < num_rows; i++ ) {
            ret.data[i] = begin[0];
            begin += num_cols;
        }

        return ret;
    }

private:
    value_type data[num_rows * num_cols];
};

//! @brief      Get identity matrix
template <typename vty__, size_t r_>
auto eye()
{
    static_matrix<vty__, r_, r_> out;

    for ( size_t i = 0; i < out.num_rows; i++ ) {
        for ( size_t j = 0; j < out.num_cols; j++ ) {
            out( i, j ) = i == j;
        }
    }

    return out;
}

//! @brief      Matrix Arithmetic Ops
//! @details
//!             ^ operator indicates element-wise multiply operation.
//!             ~ operator indicates transpose operation.
//! @{

template <typename vty_a__, typename vty_b__, size_t m_, size_t n_>
auto operator+(
  static_matrix<vty_a__, m_, n_> const& lh,
  static_matrix<vty_b__, m_, n_> const& rh ) noexcept
{
    static_matrix<matrix_impl::multiply_result_t<vty_a__, vty_b__>, m_, n_> out;
    matrix_impl::add( lh.cdesc(), rh.cdesc(), out.desc(), out.mat_size );
    return out;
}

template <typename vty_a__, typename vty_b__, size_t m_, size_t n_>
auto operator+(
  static_matrix<vty_a__, m_, n_> const& lh,
  vty_b__ const&                        rh ) noexcept
{
    static_matrix<matrix_impl::multiply_result_t<vty_a__, vty_b__>, m_, n_> out;
    matrix_impl::scalar_add( lh.cdesc(), rh, out.desc(), out.mat_size );
    return out;
}

template <typename vty_a__, typename vty_b__, size_t m_, size_t n_>
auto operator-(
  static_matrix<vty_a__, m_, n_> const& lh,
  static_matrix<vty_b__, m_, n_> const& rh ) noexcept
{
    static_matrix<matrix_impl::multiply_result_t<vty_a__, vty_b__>, m_, n_> out;
    matrix_impl::subtract( lh.cdesc(), rh.cdesc(), out.desc(), out.mat_size );
    return out;
}

template <typename vty_a__, typename vty_b__, size_t m_, size_t n_>
auto operator-(
  static_matrix<vty_a__, m_, n_> const& lh,
  vty_b__ const&                        rh ) noexcept
{
    static_matrix<matrix_impl::multiply_result_t<vty_a__, vty_b__>, m_, n_> out;
    matrix_impl::scalar_add( lh.cdesc(), -rh, out.desc(), out.mat_size );
    return out;
}

template <typename vty_a__, typename vty_b__, size_t m_, size_t n_>
auto operator^(
  static_matrix<vty_a__, m_, n_> const& lh,
  static_matrix<vty_b__, m_, n_> const& rh ) noexcept
{
    static_matrix<matrix_impl::multiply_result_t<vty_a__, vty_b__>, m_, n_> out;
    matrix_impl::elem_multiply(
      lh.cdesc(),
      rh.cdesc(),
      out.desc(),
      out.mat_size );
    return out;
}

template <typename vty_a__, typename vty_b__, size_t m_, size_t n_>
auto operator/(
  static_matrix<vty_a__, m_, n_> const& lh,
  static_matrix<vty_b__, m_, n_> const& rh ) noexcept
{
    static_matrix<matrix_impl::multiply_result_t<vty_a__, vty_b__>, m_, n_> out;
    matrix_impl::elem_divide(
      lh.cdesc(),
      rh.cdesc(),
      out.desc(),
      out.mat_size );
    return out;
}

template <typename vty_a__, typename vty_b__, size_t m_, size_t n_>
auto operator/(
  static_matrix<vty_a__, m_, n_> const& lh,
  vty_b__ const&                        rh ) noexcept
{
    static_matrix<matrix_impl::multiply_result_t<vty_a__, vty_b__>, m_, n_> out;
    matrix_impl::scalar_divide( lh.cdesc(), rh, out.desc(), out.mat_size );
    return out;
}

template <typename vty_a__, typename vty_b__, size_t m_, size_t n_>
auto operator/(
  vty_b__ const&                        rh,
  static_matrix<vty_a__, m_, n_> const& lh ) noexcept
{
    static_matrix<matrix_impl::multiply_result_t<vty_a__, vty_b__>, m_, n_> out;
    matrix_impl::scalar_divide_by_mat(
      lh.cdesc(),
      rh,
      out.desc(),
      out.mat_size );
    return out;
}

template <typename vty_a__, typename vty_b__, size_t m_, size_t n_>
auto operator*(
  static_matrix<vty_a__, m_, n_> const& lh,
  vty_b__ const&                        rh ) noexcept
{
    static_matrix<matrix_impl::multiply_result_t<vty_a__, vty_b__>, m_, n_> out;
    matrix_impl::scalar_multiply( lh.cdesc(), rh, out.desc(), out.mat_size );
    return out;
}
//
// template <typename vty_a__, typename vty_b__, size_t m_, size_t n_>
// auto operator*(
//  vty_b__ const&                        rh,
//  static_matrix<vty_a__, m_, n_> const& lh ) noexcept
//{
//    return lh * rh;
//}

template <typename vty_a__, typename vty_b__, size_t m_, size_t n_, size_t l_>
auto operator*(
  static_matrix<vty_a__, m_, n_> const& lh,
  static_matrix<vty_b__, n_, l_> const& rh ) noexcept
{
    static_matrix<matrix_impl::multiply_result_t<vty_a__, vty_b__>, m_, l_> out;
    matrix_impl::multiply( lh.cdesc(), rh.cdesc(), out.desc(), out.mat_size );
    return out;
}

// Logical operations

template <typename vty_a__, typename vty_b__, size_t m_, size_t n_>
auto operator&&(
    static_matrix<vty_a__, m_, n_> const& lh,
    static_matrix<vty_b__, m_, n_> const& rh) noexcept
{
    static_matrix<bool, m_, n_> out;
    matrix_impl::logic_and(lh.cdesc(), rh.cdesc(), out.desc(), out.mat_size);
    return out;
}

template <typename vty_a__, typename vty_b__, size_t m_, size_t n_>
auto operator||(
    static_matrix<vty_a__, m_, n_> const& lh,
    static_matrix<vty_b__, m_, n_> const& rh) noexcept
{
    static_matrix<bool, m_, n_> out;
    matrix_impl::logic_or(lh.cdesc(), rh.cdesc(), out.desc(), out.mat_size);
    return out;
}

template <typename vty_a__, typename vty_b__, size_t m_, size_t n_>
auto operator==(
  static_matrix<vty_a__, m_, n_> const& lh,
  static_matrix<vty_b__, m_, n_> const& rh ) noexcept
{
    static_matrix<bool, m_, n_> out;
    matrix_impl::equals( lh.cdesc(), rh.cdesc(), out.desc(), out.mat_size );
    return out;
}

template <typename vty_a__, typename vty_b__, size_t m_, size_t n_>
auto operator==(
  static_matrix<vty_a__, m_, n_> const& lh,
  vty_b__ const&                        rh ) noexcept
{
    static_matrix<bool, m_, n_> out;
    matrix_impl::scalar_equals( lh.cdesc(), rh, out.desc(), out.mat_size );
    return out;
} 

template <typename vty_a__, typename vty_b__, size_t m_, size_t n_>
auto operator!=(
  static_matrix<vty_a__, m_, n_> const& lh,
  static_matrix<vty_b__, m_, n_> const& rh ) noexcept
{
    static_matrix<bool, m_, n_> out;
    matrix_impl::not_equals( lh.cdesc(), rh.cdesc(), out.desc(), out.mat_size );
    return out;
}

template <typename vty_a__, typename vty_b__, size_t m_, size_t n_>
auto operator!=(
  static_matrix<vty_a__, m_, n_> const& lh,
  vty_b__ const&                        rh ) noexcept
{
    static_matrix<bool, m_, n_> out;
    matrix_impl::scalar_neq( lh.cdesc(), rh, out.desc(), out.mat_size );
    return out;
}

template <typename vty_a__, typename vty_b__, size_t m_, size_t n_>
auto operator>(
  static_matrix<vty_a__, m_, n_> const& lh,
  static_matrix<vty_b__, m_, n_> const& rh ) noexcept
{
    static_matrix<bool, m_, n_> out;
    matrix_impl::greater_than(
      lh.cdesc(),
      rh.cdesc(),
      out.desc(),
      out.mat_size );
    return out;
}

template <typename vty_a__, typename vty_b__, size_t m_, size_t n_>
auto operator>(
  static_matrix<vty_a__, m_, n_> const& lh,
  vty_b__ const&                        rh ) noexcept
{
    static_matrix<bool, m_, n_> out;
    matrix_impl::scalar_gt( lh.cdesc(), rh, out.desc(), out.mat_size );
    return out;
}

template <typename vty_a__, typename vty_b__, size_t m_, size_t n_>
auto operator>=(
  static_matrix<vty_a__, m_, n_> const& lh,
  static_matrix<vty_b__, m_, n_> const& rh ) noexcept
{
    static_matrix<bool, m_, n_> out;
    matrix_impl::greater_or_equal(
      lh.cdesc(),
      rh.cdesc(),
      out.desc(),
      out.mat_size );
    return out;
}

template <typename vty_a__, typename vty_b__, size_t m_, size_t n_>
auto operator<(
  static_matrix<vty_a__, m_, n_> const& lh,
  static_matrix<vty_b__, m_, n_> const& rh ) noexcept
{
    static_matrix<bool, m_, n_> out;
    matrix_impl::less_than( lh.cdesc(), rh.cdesc(), out.desc(), out.mat_size );
    return out;
}

template <typename vty_a__, typename vty_b__, size_t m_, size_t n_>
auto operator<=(
  static_matrix<vty_a__, m_, n_> const& lh,
  static_matrix<vty_b__, m_, n_> const& rh ) noexcept
{
    static_matrix<bool, m_, n_> out;
    matrix_impl::less_or_equal(
      lh.cdesc(),
      rh.cdesc(),
      out.desc(),
      out.mat_size );
    return out;
}

template <typename vty_a__, typename vty_b__, size_t m_, size_t n_>
auto operator!( static_matrix<vty_a__, m_, n_> const& lh ) noexcept
{
    static_matrix<bool, m_, n_> out;
    matrix_impl::not( lh.cdesc(), out.desc(), out.mat_size );
    return out;
}

//! @}

//! @}
//! @}
//! @}
}} // namespace upp::math
