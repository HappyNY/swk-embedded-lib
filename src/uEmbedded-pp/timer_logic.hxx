#pragma once
#include <list>
#include "static_fslist.hxx"
#include "timer_logic__.hxx"

namespace upp {
//! @addtogroup uEmbedded_Cpp
//! @{
//! @weakgroup  uEmbedded_Cpp_TimerLogic
//! @{

//! \brief      Simple aliasing for static timer logics
template <typename tick_ty__, typename size_ty__, size_t num_tim__>
using static_timer_logic = timer_logic<
    tick_ty__,
    static_fslist<timer_logic_desc<tick_ty__>, size_ty__, num_tim__>>;

//! \brief      Simple aliasing for dynamic timer logics
template <typename tick_ty__>
using linked_timer_logic
    = timer_logic<tick_ty__, std::list<timer_logic_desc<tick_ty__>>>;
//! @}
//! @}
} // namespace upp