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
#include <new>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <utility>

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

template <typename nty_>
class fslist_alloc_base {
protected:
    using size_type       = nty_;
    using difference_type = ptrdiff_t;
    using node_type       = fslist_node<size_type>;
    enum { NODE_NONE = (size_type)-1 };

    fslist_alloc_base( size_type capacity, node_type* narray ) noexcept : capacity_( capacity ), narray_( narray ), size_( 0 ), head_( NODE_NONE ), tail_( NODE_NONE ), idle_( 0 )
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

    size_type alloc_node() noexcept
    {
        uassert( size_ < capacity_ );
        auto& n = narray_[idle_];
        n.cur_  = idle_;
        idle_   = n.nxt_;
        ++size_;
        return n.cur_;
    }

    void insert_node( size_type i, size_type at ) noexcept
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

    void dealloc_node( size_type i ) noexcept
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

    size_type head() const noexcept { return head_; }
    size_type tail() const noexcept { return tail_; }

    size_type next( size_type n ) const noexcept { return narray_[n].nxt_; }
    size_type prev( size_type n ) const noexcept { return narray_[n].prv_; }

    bool valid_node( size_type n ) const noexcept { return n != NODE_NONE && narray_[n].cur_ != NODE_NONE; }

public:
    size_type capacity() const noexcept { return capacity_; }
    size_type size() const noexcept { return size_; }

    template <typename dty_, typename nty_>
    friend class fslist_const_iterator;

private:
    size_type size_;
    size_type capacity_;
    size_type head_;
    size_type tail_;

    size_type  idle_;
    node_type* narray_;
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
    using container_type    = fslist_alloc_base<nty_>;
    enum { NODE_NONE = (size_type)-1 };

    fslist_const_iterator<dty_, nty_>& operator++() noexcept;
    fslist_const_iterator<dty_, nty_>  operator++( int ) noexcept;
    fslist_const_iterator<dty_, nty_>& operator--() noexcept;
    fslist_const_iterator<dty_, nty_>  operator--( int ) noexcept;
    reference                          operator*() const noexcept;
    pointer                            operator->() const noexcept;
    friend bool                        operator!=( fslist_const_iterator const& a, fslist_const_iterator const& b ) noexcept;

private:
    container_type const* container_;
    size_type             cur_;

private:
    template <typename dty_, typename nty_>
    friend class fslist_base;
};

template <typename dty_, typename nty_>
class fslist_iterator : public fslist_const_iterator<dty_, nty_> {
public:
    using iterator_category = std::bidirectional_iterator_tag;
    using value_type        = dty_;
    using difference_type   = ptrdiff_t;
    using pointer           = dty_*;
    using reference         = dty_&;
    using size_type         = nty_;
    using container_type    = fslist_alloc_base<nty_>;
    using super             = fslist_const_iterator;

    fslist_iterator<dty_, nty_>& operator++() noexcept
    {
        return static_cast<fslist_iterator<dty_, nty_>&>( super::operator++() );
    }
    fslist_iterator<dty_, nty_> operator++( int ) noexcept
    {
        auto r = *this;
        return ++r;
    }

    fslist_iterator<dty_, nty_>& operator--() noexcept
    {
        return static_cast<fslist_iterator&>( super::operator--() );
    }
    fslist_iterator<dty_, nty_> operator--( int ) noexcept
    {
        auto r = *this;
        return ++r;
    }
    reference operator*() const noexcept
    {
        return const_cast<reference>( super::operator*() );
    }
    pointer operator->() const noexcept
    {
        return const_cast<pointer>( super::operator->() );
    }

    bool operator!=( fslist_iterator const& b ) const noexcept { return (super&)*this != (super&)b; }
};

template <typename dty_, typename nty_>
bool operator!=( fslist_const_iterator<dty_, nty_> const& a, fslist_const_iterator<dty_, nty_> const& b ) noexcept
{
    return a.container_ != b.container_ || a.cur_ != b.cur_;
}

//! Base class for list
template <typename dty_,
          typename nty_ = size_t>
class fslist_base : public fslist_alloc_base<nty_> {
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
    using iterator        = fslist_iterator<value_type, size_type>;
    using const_iterator  = fslist_const_iterator<value_type, size_type>;
    enum { NODE_NONE = (size_type)-1 };

public:
    ~fslist_base() noexcept
    {
        for ( size_type i = head(); i != NODE_NONE; i = next( i ) ) {
            varray_[i].~value_type();
        }
    }

    fslist_base( size_type capacity, pointer varray, node_type* narray ) noexcept : super_type( capacity, narray ), varray_( varray )
    {
    }

    template <typename... arg_>
    reference emplace_front( arg_&&... args ) noexcept
    {
        auto n = alloc_node();
        insert_node( n, head() );
        auto p = new ( &varray_[n] ) value_type( std::forward<arg_>( args )... );
        return *p;
    }

    template <typename... arg_>
    reference emplace_back( arg_&&... args ) noexcept
    {
        auto n = alloc_node();
        insert_node( n, NODE_NONE );
        auto p = new ( &varray_[n] ) value_type( std::forward<arg_>( args )... );
        return *p;
    }

    void push_back( const_reference arg ) noexcept
    {
        emplace_back( arg );
    }

    void push_front( const_reference arg ) noexcept
    {
        emplace_front( arg );
    }

    const_iterator cbegin() const noexcept
    {
        const_iterator i;
        i.container_ = this;
        i.cur_       = head();
        return i;
    }

    const_iterator cend() const noexcept
    {
        const_iterator i;
        i.container_ = this;
        i.cur_       = NODE_NONE;
        return i;
    }

    iterator begin() noexcept { return static_cast<iterator&>( cbegin() ); }
    iterator end() noexcept { return static_cast<iterator&>( cend() ); }

    const reference front() const noexcept
    {
        uassert( valid_node( head() ) );
        return varray_[head()];
    }

    const reference back() const noexcept
    {
        uassert( valid_node( tail() ) );
        return varray_[tail()];
    }

    reference front() noexcept
    {
        uassert( valid_node( head() ) );
        return varray_[head()];
    }

    reference back() noexcept
    {
        uassert( valid_node( tail() ) );
        return varray_[tail()];
    }

    

private:
    template <typename dty_, typename nty_>
    friend class fslist_const_iterator;

    pointer get_arg( size_type node ) noexcept
    {
        uassert( node != NODE_NONE );
        uassert( valid_node( node ) );
        return varray_ + node;
    }

private:
    pointer varray_;
};

template <typename dty_, typename nty_>
inline fslist_const_iterator<dty_, nty_>& fslist_const_iterator<dty_, nty_>::operator++() noexcept
{
    uassert( container_ && cur_ != NODE_NONE );
    cur_ = container_->next( cur_ );
    return *this;
}

template <typename dty_, typename nty_>
inline fslist_const_iterator<dty_, nty_> fslist_const_iterator<dty_, nty_>::operator++( int ) noexcept
{
    auto ret = *this;
    return ++ret;
}

template <typename dty_, typename nty_>
inline fslist_const_iterator<dty_, nty_>& fslist_const_iterator<dty_, nty_>::operator--() noexcept
{
    uassert( container_ && cur_ != container_.head() );
    if ( cur_ == NODE_NONE ) {
        cur_ = container_->tail();
    }
    else {
        cur_ = container_->prev( cur_ );
    }
    return *this;
}

template <typename dty_, typename nty_>
inline fslist_const_iterator<dty_, nty_> fslist_const_iterator<dty_, nty_>::operator--( int ) noexcept
{
    auto ret = *this;
    return --ret;
}

template <typename dty_, typename nty_>
inline typename fslist_const_iterator<dty_, nty_>::reference fslist_const_iterator<dty_, nty_>::operator*() const noexcept
{
    auto c = static_cast<fslist_base<dty_, nty_>*>( const_cast<fslist_alloc_base<nty_>*>( container_ ) );
    return *c->get_arg( cur_ );
}

template <typename dty_, typename nty_>
inline typename fslist_const_iterator<dty_, nty_>::pointer fslist_const_iterator<dty_, nty_>::operator->() const noexcept
{
    auto c = static_cast<fslist_base<dty_, nty_>>( container_ );
    return c->get_arg( cur_ );
}

}; // namespace impl
}; // namespace upp