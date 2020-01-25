//! A list base class
//!
//! @author Seungwoo Kang (ki6080@gmail.com)
//! @copyright Copyright (c) 2019. Seungwoo Kang. All rights reserved.
//!
//! @details
//! This class implements thread-unsafe lightweight linked list
#pragma once
#include "../uEmbedded/uassert.h"
#include <iterator>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <utility>
#include <new>

namespace upp {
namespace impl {

template <typename nty_>
struct fslist_node {
    nty_ nxt_;
    nty_ prv_;

    nty_ cur() { return cur_; }

private:
    nty_ cur_;
    template <typename t_>
    friend class fslist_alloc_base;
};

//! list iterator definition
template <typename dty_, typename nty_>
class fslist_const_iterator {
public:
    using iterator_category = std::bidirectional_iterator_tag;
    using value_type        = dty_;
    using difference_type   = ptrdiff_t;
    using pointer           = dty_ const*;
    using reference         = dty_ const&;
    using size_type         = nty_;

private:
    intptr_t  container_;
    size_type cur_;
};

template <typename nty_>
class fslist_alloc_base {
protected:
    using size_type       = nty_;
    using difference_type = ptrdiff_t;
    using node_type       = fslist_node<size_type>;
    enum { NODE_NONE = (size_type)-1 };

    fslist_alloc_base( size_type capacity, node_type* narray ) : capacity_( capacity ), narray_( narray ), size_( 0 ), head_( NODE_NONE ), tail_( NODE_NONE ), idle_( 0 )
    {
        // Link all available nodes
        for ( size_t i = 0; i < capacity; i++ ) {
            auto p  = narray_ + i;
            p->nxt_ = i + 1;
            p->cur_ = NODE_NONE;
            p->prv_ = i - 1;
        }
        narray_[0].prv_             = NODE_NONE;
        narray_[capacity_ - 1].nxt_ = NODE_NONE;
    }

    size_type alloc_node()
    {
        uassert( size_ < capacity_ );
        auto& n = narray_[idle_];
        n.cur_  = idle_;
        idle_   = n.nxt_;
        ++size_;
        return n.cur_;
    }

    void insert_node( size_type i, size_type at )
    {
        node_type& n = narray_[i];
        uassert( n.cur_ != NODE_NONE );
        if ( at == NODE_NONE ) {
            n.nxt_ = NODE_NONE;
            n.prv_ = tail_;
            if ( tail_ != NODE_NONE ) {
                narray_[tail_].nxt_ = i;
            }
            tail_ = i;
        }
        else {
            if ( at == head_ ) {
                head_ = i;
            }
            node_type& n_at = narray_[at];
            uassert( n_at.cur_ != NODE_NONE );
            n.nxt_    = at;
            n.prv_    = n_at.prv_;
            n_at.prv_ = i;
            if ( n_at.prv_ != NODE_NONE ) {
                narray_[n_at.prv_].nxt_ = i;
            }
        }
    }

    void dealloc_node( size_type i )
    {
        auto& n = narray_[i];
        uassert( n.cur_ != NODE_NONE );
        uassert( i >= 0 && i < capacity_ );

        if ( n.nxt_ != NODE_NONE ) {
            narray_[n.nxt_].prv_ = n.prv_;
        }
        if ( n.prv_ != NODE_NONE ) {
            narray_[n.prv_].nxt_ = n.nxt_;
        }
        n.prv_              = NODE_NONE;
        n.cur_              = NODE_NONE;
        n.nxt_              = idle_;
        narray_[idle_].prv_ = i;
        idle_               = i;
        --size_;
    }

    size_type head() const { return head_; }
    size_type tail() const { return tail_; }
    size_type capacity() const { return capacity_; }
    size_type size() const { return size_; }

private:
    size_type size_;
    size_type capacity_;
    size_type head_;
    size_type tail_;

    size_type  idle_;
    node_type* narray_;
};

//! Base class for list
template <typename dty_,
          typename nty_ = size_t>
class fslist_base : fslist_alloc_base<nty_> {
public:
    using super_type      = fslist_alloc_base<nty_>;
    using value_type      = dty_;
    using size_type       = nty_;
    using difference_type = ptrdiff_t;
    using pointer         = value_type*;
    using reference       = value_type&;
    using const_pointer   = value_type const*;
    using const_reference = value_type const;
    using node_type       = fslist_node<size_type>;
    enum { NODE_NONE = (size_type)-1 };

public:
    fslist_base( size_type capacity, pointer varray, node_type* narray ) : super_type( capacity, narray ), varray_( varray )
    {
    }

    template <typename arg_>
    void push_front( arg_&& ty )
    {
        uassert( size_ < capacity_ );
        auto& n = alloc_node();
    }

private:
protected:
    pointer varray_;
};

}; // namespace impl
}; // namespace upp