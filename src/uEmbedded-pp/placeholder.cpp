//! Placeholder source file to activate intellisense
//!
//! @author Seungwoo Kang (ki6080@gmail.com)
//! @copyright Copyright (c) 2019. Seungwoo Kang. All rights reserved.
//!
//! @details
#include "__falist_base.hpp"

using namespace upp::impl;

void fslist_base_build_test()
{
    fslist_base<double> f{ 10, NULL, NULL };
    f.push_front( 3 );
}