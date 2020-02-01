#pragma once
#include <algorithm>
#include <functional>
#include <stdint.h>
#include "../uEmbedded/uassert.h"
namespace upp {
//! @addtogroup uEmbedded_Cpp
//! @{
//! @defgroup   uEmbedded_Cpp_TimerLogic
//! @brief      Logical timer management class
//! @{

using timer_cb_t = void ( * )( void* );
enum
{
    TIMER_INVALID = -1
};

namespace impl {

template <typename tick_ty__>
struct timer_handle
{
    tick_ty__ id_;
    tick_ty__ time_;
};

} // namespace impl

template <typename tick_ty__>
struct timer_logic_desc
{
    tick_ty__  id_;
    tick_ty__  trigger_at_;
    void*      obj_;
    timer_cb_t cb_;
};

//! @brief      Logical timer management class
//! @details
//!              Regardless of hardware, it abstracts timer behavior logically.
//!             All timer events are triggered by calling the timer's update ()
//!             function, and time information is obtained internally by the
//!             tick_function callback. \n
//!              The simplest implementation is to run a timer class with system
//!             time using tick_type__ with a high resolution. \n
//! @tparam tick_ty__
//!              A data type that stores timer ticks. It must be in a
//!             format that supports common arithmetic and comparison
//!             operations. \n
//!              The timer does not have an instance of tick_ty__ internally and
//!             only performs a time comparison with tick_function__.If you use
//!             tick_ty__ with a lower maximum, you can implement a timer by
//!             flexibly adjusting the time origin.
//! @tparam list_container__
//!              @ref upp::impl::timer_logic_desc A dataset with a list-type
//!             interface that stores it. \n
//!              e.g. std::list<upp::impl::timer_logic_desc<tick_t>> \n
//!              Because the timer uses only the standard list interface
//!             internally, you can flexibly use any data structure that
//!             supports the insert() function.
template <typename tick_ty__, typename list_container__>
class timer_logic
{
public:
    using desc_type      = timer_logic_desc<tick_ty__>;
    using tick_type      = tick_ty__;
    using tick_fnc_type  = std::function<tick_type( void )>;
    using container_type = list_container__;
    using handle_type    = impl::timer_handle<tick_ty__>;

public:
    tick_fnc_type const& tick_function() const noexcept { return tick_; }
    template <class tick_fnc__>
    void tick_function( tick_fnc__&& v ) noexcept
    {
        tick_ = std::forward<tick_fnc__>( v );
    }

    //! @brief  Add a timer instance
    //! @param delay
    //!         Timer delay in timer tick dimension
    //! @param obj
    //!         A object will be delivered with timer callback
    //! @param callback
    //!         Timer callback event.
    //! @returns
    //!         Timer handle of newly allocated timer instance.
    handle_type add( tick_type delay, void* obj, timer_cb_t callback ) noexcept
    {
        uassert( is_updating_ == false );
        uassert( tick_ );
        uassert( capacity() );

        desc_type d;
        d.trigger_at_ = delay + tick_();
        d.cb_         = callback;
        d.obj_        = obj;
        d.id_         = id_gen_++;

        auto at = std::find_if( node_.begin(), node_.end(), [&d]( auto& a ) {
            return d.trigger_at_ < a.trigger_at_;
        } );

        node_.insert( at, d );

        handle_type ret;
        ret.id_   = d.id_;
        ret.time_ = d.trigger_at_;
        return ret;
    }

    //! @brief      Removes allocated timer.
    bool remove( handle_type const& t ) noexcept
    {
        uassert( is_updating_ == false );
        auto it = find_( t );
        if ( it != node_.end() ) {
            node_.erase( it );
            return true;
        }
        else {
            return false;
        }
    }

    //! @brief      Get next trigger time
    //! @returns
    //!              the trigger time of the earliest element in the timer
    //!             instance waiting for a trigger.
    tick_type next_trig() const noexcept
    {
        if ( empty() ) {
            return TIMER_INVALID;
        }
        return node_.front().trigger_at_;
    }

    //! @brief Browse existing timer instance
    //! @param[out] out Will return timer descriptor
    //! @returns true if given timer node is valid.
    bool browse( handle_type const& t, desc_type& out ) const noexcept
    {
        auto it = find_( t );
        if ( it == node_.cend() ) {
            return false;
        }
        out = *it;
        return true;
    }

    //! @brief      Update timer.
    //! @details
    //!              It compares sequentially with the time returned by
    //!             tick_function from the front of the active timer node. \n
    //!              The timer node is always sorted, so even after performing
    //!             an update, the timer node is always sorted. \n
    //!              Also, it always performs a comparison with the frontmost
    //!             element of the timer node, so it's safe to recursively
    //!             assign new timers inside the timer event. \n
    //! @warning
    //!              Because it is not reentrant, ensure extra atomicity in a
    //!             multi-threaded environment.
    //! @returns @ref next_trig()
    //! \{
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
    template <class callable_lock__, class callable_unlock__>
    tick_type
    update_lock( callable_lock__&& lock, callable_unlock__&& unlock ) noexcept
    {
        typename container_type::const_iterator it;

        for ( ;; ) {
            is_updating_ = true;
            it           = node_.begin();

            if ( it == node_.end() || it->trigger_at_ > tick_() ) {
                break;
            }

            auto cb  = it->cb_;
            auto obj = it->obj_;

            lock();
            node_.pop_front();
            unlock();
            is_updating_ = false;

            cb( obj );
        }

        is_updating_ = false;
        return next_trig();
    }
    //! \}

    //! @brief      Clear all timer instances
    void clear() noexcept { node_.clear(); }

    //! @brief      Get available space for new timer nodes
    size_t capacity() const noexcept { return node_.max_size() - node_.size(); }

    //! @brief      Get number of active timers
    size_t size() const noexcept { return node_.size(); }

    //! @brief      Check if timer node is empty.
    bool empty() const noexcept { return node_.empty(); }

private:
    typename container_type::const_iterator find_( handle_type const& h ) const
    {
        auto       beg = node_.cbegin();
        auto const end = node_.cend();
        for ( ; beg != end && h.time_ < beg->trigger_at_; ++beg ) {
            if ( h.id_ == beg->id_ )
                return beg;
        }
        return end;
    }

private:
    volatile bool  is_updating_ = false;
    container_type node_;
    tick_fnc_type  tick_;
    tick_type      id_gen_ = 0;
};

//! @}
//! @}
} // namespace upp