#pragma once
#include <string>
#include "__matrix_base.hpp"

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

} // namespace std
