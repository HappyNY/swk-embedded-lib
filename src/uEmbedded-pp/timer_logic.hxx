#pragma once
#include "../uEmbedded/uassert.h"
#include <algorithm>
#include <functional>
#include <stdint.h>

namespace upp {
using timer_cb_t = void ( * )( void* );
enum { TIMER_INVALID = -1 };

namespace impl {

template <typename tick_ty__>
struct timer_handle {
    tick_ty__ id_;
    tick_ty__ time_;
};

} // namespace impl

template <typename tick_ty__>
struct timer_logic_desc {
    tick_ty__  id_;
    tick_ty__  trigger_at_;
    void*      obj_;
    timer_cb_t cb_;
};

template <typename tick_ty__,
          typename list_container__>
class timer_logic {
public:
    using desc_type      = timer_logic_desc<tick_ty__>;
    using tick_type      = tick_ty__;
    using tick_fnc_type  = std::function<tick_type( void )>;
    using container_type = list_container__;
    using handle_type    = impl::timer_handle<tick_ty__>;

public:
    tick_fnc_type const& tick_function() const noexcept { return tick_; }
    template <class tick_fnc__>
    void tick_function( tick_fnc__&& v ) noexcept { tick_ = std::forward<tick_fnc__>( v ); }

    handle_type add( tick_type delay, void* obj, timer_cb_t callback ) noexcept
    {
        uassert( tick_ );
        uassert( capacity() );

        desc_type d;
        d.trigger_at_ = delay + tick_();
        d.cb_         = callback;
        d.obj_        = obj;
        d.id_         = id_gen_++;

        auto at = std::find_if(
            node_.begin(), node_.end(),
            [&d]( auto& a ) { return d.trigger_at_ < a.trigger_at_; } );

        node_.insert( at, d );

        handle_type ret;
        ret.id_   = d.id_;
        ret.time_ = d.trigger_at_;
        return ret;
    }

    bool remove( handle_type const& t ) noexcept
    {
        auto it = find_( t );
        if ( it != node_.end() ) {
            node_.erase( it );
            return true;
        }
        else {
            return false;
        }
    }

    tick_type next_trig() const noexcept
    {
        if ( empty() ) {
            return TIMER_INVALID;
        }
        return node_.front().trigger_at_;
    }

    bool browse( handle_type const& t, desc_type& out ) const noexcept
    {
        auto it = find_( t );
        if ( it == node_.cend() ) {
            return false;
        }
        out = *it;
        return true;
    }

    tick_type update() noexcept
    {
        for ( auto it = node_.begin();
              it != node_.end() && it->trigger_at_ <= tick_();
              it = node_.begin() ) {
            auto cb  = it->cb_;
            auto obj = it->obj_;

            node_.pop_front();
            cb( obj );
        }

        return next_trig();
    }

    void clear() noexcept
    {
        node_.clear();
    }

    size_t capacity() const noexcept { return node_.max_size() - node_.size(); }
    size_t size() const noexcept { return node_.size(); }
    bool   empty() const noexcept { return node_.empty(); }

private:
    typename container_type::const_iterator
    find_( handle_type const& h ) const
    {
        auto       beg = node_.cbegin();
        auto const end = node_.cend();
        for ( ; beg != end
                && h.time_ < beg->trigger_at_;
              ++beg ) {
            if ( h.id_ == beg->id_ )
                return beg;
        }
        return end;
    }

private:
    container_type node_;
    tick_fnc_type  tick_;
    tick_type      id_gen_ = 0;
};

} // namespace upp