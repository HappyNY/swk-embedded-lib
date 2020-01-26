#pragma once
#include "static_fslist.hxx"
#include <algorithm>
#include <functional>
#include <optional>
#include <stdint.h>

namespace upp {
using timer_cb_t = void ( * )( void* );
enum { TIMER_INVALID = -1 };

namespace impl {

template <typename tick_ty__>
struct timer_logic_desc {
    tick_ty__  id_;
    tick_ty__  trigger_at_;
    void*      obj_;
    timer_cb_t cb_;
};

template <typename tick_ty__, typename it__>
struct timer_handle {
    tick_ty__ id_;
    it__      at_;
};

} // namespace impl

template <typename tick_ty__,
          typename size_ty__,
          size_t max_node__>
class timer_logic {
public:
    using desc_type      = impl::timer_logic_desc<tick_ty__>;
    using tick_type      = tick_ty__;
    using size_type      = size_ty__;
    using tick_fnc_type  = std::function<tick_type( void )>;
    using container_type = upp::static_fslist<desc_type, size_type, max_node__>;
    using handle_type    = impl::timer_handle<tick_ty__, typename container_type::const_iterator>;

public:
    tick_fnc_type const& tick_function() const { return tick_; }
    template <class tick_fnc__>
    void tick_function( tick_fnc__&& v ) { tick_ = std::forward<tick_fnc__>( v ); }

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

        auto it = node_.insert( at, d );

        handle_type ret;
        ret.at_ = it;
        ret.id_ = d.id_;

        return ret;
    }

    bool remove( handle_type t ) noexcept
    {
        if ( t.at_.valid() &&
             t.at_->id_ == t.id_ ) {
            node_.erase( t.at_ );
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

    std::optional<desc_type> browse( handle_type t ) const noexcept
    {
        if ( t.at_ &&
             t.at_->id_ == t.id_ ) {
            return *t.at_;
        }
        return {};
    }

    tick_type update() noexcept
    {
        for ( auto it = node_.begin();
              it && it->trigger_at_ <= tick_();
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

    size_type capacity() const noexcept { return node_.capacity(); }
    size_type size() const noexcept { return node_.size(); }
    bool      empty() const noexcept { return node_.empty(); }

private:
    container_type node_;
    tick_fnc_type  tick_;
    tick_type      id_gen_ = 0;
};
} // namespace upp