#pragma once
#include "static_fslist.hxx"
#include "timer_logic.hxx"

namespace upp {
template <typename tick_ty__, typename size_ty__, size_t num_tim__>
using static_timer_logic = timer_logic<
    tick_ty__,
    static_fslist<timer_logic_desc<tick_ty__>, size_ty__, num_tim__>>;
}