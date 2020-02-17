#include "__fslist_base.hxx"

namespace upp {
//! @addtogroup uEmbedded_Cpp
//! @{
//! @weakgroup  uEmbedded_Cpp_FreeSpaceList
//! @{

//! \brief       A simple wrapper class that allocates static memory for nodes
//!             and data buffers.
template <typename value_ty__, typename size_ty__, size_t cap__>
class static_fslist : public impl::fslist_base<value_ty__, size_ty__>
{
public:
    using super           = impl::fslist_base<value_ty__, size_ty__>;
    using value_type      = value_ty__;
    using size_type       = size_ty__;
    using difference_type = ptrdiff_t;
    using pointer         = value_type*;
    using reference       = value_type&;
    using const_pointer   = value_type const*;
    using const_reference = value_type const;
    using iterator        = typename super::iterator;
    using const_iterator  = typename super::const_iterator;

public:
    static_fslist() : super( cap__, &vbuf[0], &nbuf[0] ) { }

private:
    value_ty__                   vbuf[cap__];
    impl::fslist_node<size_ty__> nbuf[cap__];
};

//! @}
//! @}
} // namespace upp