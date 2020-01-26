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
    enum { NODE_NONE = (nty_)-1 };
    nty_ nxt_;
    nty_ prv_;

    nty_ cur() { return cur_; }

    fslist_node<nty_>& prev() { return by_( prv_ ); }
    fslist_node<nty_>& next() { return by_( nxt_ ); }

private:
    fslist_node<nty_>& by_( int absolute )
    {
        uassert( nxt_ != NODE_NONE && prv_ != NODE_NONE );
        return *( this + cur_ - absolute );
    }

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

    fslist_alloc_base( size_type capacity, node_type* narray ) noexcept : size_( 0 ), capacity_( capacity ), head_( NODE_NONE ), tail_( NODE_NONE ), idle_( 0 ), narray_( narray )
    {
        // Link all available nodes
        for ( size_t i = 0; i < capacity; i++ ) {
            auto p  = narray_ + i;
            p->nxt_ = static_cast<size_type>( i + 1 );
            p->cur_ = NODE_NONE;
            p->prv_ = static_cast<size_type>( i - 1 );
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
            else { // tail is empty == head is empty
                head_ = i;
            }
            tail_ = i;
        }
        else {
            if ( at == head_ ) {
                head_ = i;
            }
            node_type& n_at = narray_[at];
            uassert( n_at.cur_ != NODE_NONE );
            n.nxt_ = at;
            n.prv_ = n_at.prv_;
            if ( n_at.prv_ != NODE_NONE ) {
                n_at.prev().nxt_ = i;
            }
            n_at.prv_ = i;
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
        else { // It's tail
            tail_ = n.prv_;
        }

        if ( n.prv_ != NODE_NONE ) {
            narray_[n.prv_].nxt_ = n.nxt_;
        }
        else { // It's head
            head_ = n.nxt_;
        }

        if ( idle_ != NODE_NONE ) {
            narray_[idle_].prv_ = i;
        }
        n.prv_ = NODE_NONE;
        n.cur_ = NODE_NONE;
        n.nxt_ = idle_;
        idle_  = i;
        --size_;
    }

    size_type head() const noexcept { return head_; }
    size_type tail() const noexcept { return tail_; }

    size_type next( size_type n ) const noexcept { return narray_[n].nxt_; }
    size_type prev( size_type n ) const noexcept { return narray_[n].prv_; }

    bool valid_node( size_type n ) const noexcept { return n != NODE_NONE && narray_[n].cur_ != NODE_NONE; }

public:
    size_type max_size() const noexcept { return capacity_; }
    size_type capacity() const noexcept { return capacity_ - size_; }
    size_type size() const noexcept { return size_; }
    bool      empty() const noexcept { return size_ == 0; }

    template <typename ty1_, typename ty_2>
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

    bool operator!=( const fslist_const_iterator<dty_, nty_>& r ) const noexcept { return r.container_ != container_ || r.cur_ != cur_; }
    bool operator==( const fslist_const_iterator<dty_, nty_>& r ) const noexcept { return r.container_ == container_ && r.cur_ == cur_; }

    bool valid() const noexcept { return container_->valid_node( cur_ ); }
    operator bool() const noexcept { return valid(); }

    size_type fs_idx__() const noexcept { return cur_; }

private:
    container_type const* container_;
    size_type             cur_;

private:
    template <typename ty1_, typename ty2_>
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
    using super             = fslist_const_iterator<dty_, nty_>;

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
    bool operator==( fslist_iterator const& b ) const noexcept { return (super&)*this == (super&)b; }

    operator super() const noexcept
    {
        return static_cast<super&> * this;
    }
};

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
    using super           = fslist_alloc_base<nty_>;
    enum { NODE_NONE = (size_type)-1 };

public:
    ~fslist_base() noexcept
    {
        clear();
    }

    void clear() noexcept
    {
        for ( size_type i = super::head(); i != NODE_NONE; ) {
            varray_[i].~value_type();
            auto k = i;
            i      = super::next( i );
            super::dealloc_node( k );
        }
    }

    fslist_base( size_type capacity, pointer varray, node_type* narray ) noexcept : super_type( capacity, narray ), varray_( varray )
    {
    }

    template <typename... arg_>
    reference emplace_front( arg_&&... args ) noexcept
    {
        auto n = super::alloc_node();
        super::insert_node( n, super::head() );
        auto p = new ( varray_ + n ) value_type( std::forward<arg_>( args )... );
        return *p;
    }

    template <typename... arg_>
    reference emplace_back( arg_&&... args ) noexcept
    {
        auto n = super::alloc_node();
        super::insert_node( n, NODE_NONE );
        auto p = new ( varray_ + n ) value_type( std::forward<arg_>( args )... );
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
        i.cur_       = super::head();
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

    const_reference front() const noexcept
    {
        uassert( valid_node( super::head() ) );
        return varray_[super::head()];
    }

    const_reference back() const noexcept
    {
        uassert( valid_node( super::tail() ) );
        return varray_[super::tail()];
    }

    reference front() noexcept
    {
        uassert( valid_node( super::head() ) );
        return varray_[super::head()];
    }

    reference back() noexcept
    {
        uassert( valid_node( super::tail() ) );
        return varray_[super::tail()];
    }

    template <typename... ty__>
    iterator emplace( const_iterator pos, ty__&&... args ) noexcept
    {
        uassert( super::size() < super::max_size() );
        auto n = super::alloc_node();
        super::insert_node( n, pos.cur_ );

        auto p = new ( varray_ + n ) value_type( std::forward<ty__>( args )... );

        const_iterator r;
        r.container_ = this;
        r.cur_       = n;
        return static_cast<iterator&>( r );
    }

    iterator insert( const_iterator pos, const value_type& arg ) noexcept
    {
        return emplace( pos, arg );
    }

    template <typename it_>
    iterator insert( const_iterator pos, it_ begin, it_ end ) noexcept
    {
        for ( ; begin != end; ++begin ) {
            pos = emplace( pos, *begin );
        }
    }

    void pop_back() noexcept
    {
        release( super::tail() );
    }

    void pop_front() noexcept
    {
        release( super::head() );
    }

    void erase( const_iterator pos ) noexcept
    {
        release( pos.cur_ );
    }

    const_pointer at__( size_type fs_idx ) const noexcept
    {
        return super::valid_node( fs_idx ) ? varray_ + fs_idx : nullptr;
    }

    pointer at__( size_type fs_idx ) noexcept
    {
        return super::valid_node( fs_idx ) ? varray_ + fs_idx : nullptr;
    }

private:
    template <typename ty1_, typename ty2_>
    friend class fslist_const_iterator;

    void release( size_type n )
    {
        uassert( n != NODE_NONE );
        varray_[n].~value_type();
        super::dealloc_node( n );
    }

    pointer get_arg( size_type node ) noexcept
    {
        uassert( node != NODE_NONE );
        uassert( super::valid_node( node ) );
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
    auto c = static_cast<fslist_base<dty_, nty_>*>( const_cast<fslist_alloc_base<nty_>*>( container_ ) );
    return c->get_arg( cur_ );
}

}; // namespace impl
}; // namespace upp