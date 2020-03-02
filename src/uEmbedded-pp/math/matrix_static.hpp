#pragma once
#include "__matrix_base.hpp"
#include <cstdlib>

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
    enum
    {
        num_rows = row__,
        num_cols = col__,
        mat_size = num_rows * num_cols * sizeof( vty__ )
    };
    using value_type = vty__;
    using cdesc_type = matrix_impl::cdesc<value_type>;
    using desc_type  = matrix_impl::desc<value_type>;

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

template <typename vty_a__, typename vty_b__, size_t m_, size_t n_, size_t l_>
static static_matrix<matrix_impl::multiply_result_t<vty_a__, vty_b__>, m_, l_>
operator*(
  static_matrix<vty_a__, m_, n_> const& lh,
  static_matrix<vty_b__, n_, l_> const& rh ) noexcept
{
    static_matrix<matrix_impl::multiply_result_t<vty_a__, vty_b__>, m_, l_> out;
    matrix_impl::multiply( lh.cdesc(), rh.cdesc(), out.desc(), out.mat_size );
    return out;
}

template <typename vty_a__, typename vty_b__, size_t m_, size_t n_>
static static_matrix<matrix_impl::multiply_result_t<vty_a__, vty_b__>, m_, n_> operator+(
  static_matrix<vty_a__, m_, n_> const& lh,
  static_matrix<vty_b__, m_, n_> const& rh ) noexcept
{
    static_matrix<matrix_impl::multiply_result_t<vty_a__, vty_b__>, m_, n_> out;
    matrix_impl::add( lh.cdesc(), rh.cdesc(), out.desc(), out.mat_size );
    return out;
}

//! @brief      Scalar Ops
//! @{

//! @todo Matrix operator + with scalar
//! @todo Matrix operator - with scalar
//! @todo Matrix operator * with scalar
//! @todo Matrix operator / with scalar
//! @}

//! @}
//! @}
//! @}
}} // namespace upp::math
