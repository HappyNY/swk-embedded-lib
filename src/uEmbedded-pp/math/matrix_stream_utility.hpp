#pragma once
#include <iostream>
#include <string>
#include "__matrix_base.hpp"
#include "matrix_static.hpp"

namespace std {

template <typename vty__>
std::string to_string( upp::math::matrix_impl::cdesc<vty__> desc )
{
    std::string str;

    str += "[ ";
    for ( size_t i = 0; i < desc.row__; i++ ) {
        for ( size_t j = 0; j < desc.col__; j++ ) {
            str += std::to_string( desc.dptr__[desc.col__ * i + j] );

            if ( j + 1 != desc.col__ ) {
                str += ", ";
            }
        }
        if ( i + 1 != desc.row__ ) {
            str += "; ";
        }
    }
    str += ']';

    return str;
} // namespace std

template <typename vty__>
std::string to_string( upp::math::matrix_impl::desc<vty__> desc )
{
    return to_string( upp::math::matrix_impl::cdesc<vty__>( desc ) );
}

template <typename vty__, size_t r_, size_t c_>
std::string to_string( upp::math::static_matrix<vty__, r_, c_> const& r )
{
    return to_string( r.cdesc() );
}

template <typename vty__, size_t r_, size_t c_>
std::ostream&
operator<<( std::ostream& os, upp::math::static_matrix<vty__, r_, c_> const& r )
{
    return os << to_string( r.cdesc() );
}

} // namespace std
