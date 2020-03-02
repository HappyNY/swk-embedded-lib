#pragma once
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

public:
    //! @brief      Arithmetic operators
    //! @details
    //!             Multiply/Divide operations will be treated as element-wise.
    //! @{

    //! @brief      Addition
    template <typename ty__>
    auto operator+( static_matrix<ty__, num_rows, num_cols> const& rh ) noexcept
    {
        static_matrix<
          matrix_impl::add_result_t<value_type, ty__>,
          num_rows,
          num_cols>
          out;
        matrix_impl::add( cdesc(), rh.cdesc(), out.desc(), mat_size );
        return out;
    }

    //! @brief      Subtraction
    template <typename ty__>
    auto operator-( static_matrix<ty__, num_rows, num_cols> const& rh ) noexcept
    {
        static_matrix<
          matrix_impl::subtract_result_t<value_type, ty__>,
          num_rows,
          num_cols>
          out;
        matrix_impl::subtract( cdesc(), rh.cdesc(), out.desc(), mat_size );
        return out;
    }

    template <typename ty__, size_t o_col__>
    auto operator*( static_matrix<ty__, num_cols, o_col__> const& rh ) noexcept
    {
        static_matrix<
          matrix_impl::multiply_result_t<value_type, ty__>,
          num_rows,
          o_col__>
          out;
        matrix_impl::multiply( cdesc(), rh.cdesc(), out.desc(), mat_size );
        return out;
    }

    //! @brief      Element-wise multiplication
    template <typename ty__>
    auto operator^( static_matrix<ty__, num_rows, num_cols> const& rh ) noexcept
    {
        static_matrix<
          matrix_impl::multiply_result_t<value_type, ty__>,
          num_rows,
          num_cols>
          out;
        matrix_impl::elem_multiply( cdesc(), rh.cdesc(), out.desc(), mat_size );
        return out;
    }

    //! @brief      Element-wise division
    template <typename ty__>
    auto operator/( static_matrix<ty__, num_rows, num_cols> const& rh ) noexcept
    {
        static_matrix<
          matrix_impl::divide_result_t<value_type, ty__>,
          num_rows,
          num_cols>
          out;
        matrix_impl::elem_divide( cdesc(), rh.cdesc(), out.desc(), mat_size );
        return out;
    }

    //! @}
private:
    value_type data[num_rows * num_cols];
};

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
