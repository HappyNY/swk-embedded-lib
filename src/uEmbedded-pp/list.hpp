//! A list base class
//!
//! @author Seungwoo Kang (ki6080@gmail.com)
//! @copyright Copyright (c) 2019. Seungwoo Kang. All rights reserved.
//!
//! @details
#pragma once
#include "allocator.hpp"

namespace upp {
template <typename dty_, typename nty_>
class list_base {
public:
    using element_type = dty_;

public:
    ~list_base() {}

private:
};

}; // namespace upp

} // namespace umb