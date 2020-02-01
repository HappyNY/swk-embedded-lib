#pragma once
namespace upp {
//! @addtogroup uEmbedded_Cpp
//! @{
//! @defgroup uEmbedded_Cpp_Delegate
//! @brief Delegate
//! @details
//!   A delegate class that supports any pointer class that provides the
//!  following interface; lock(), and operator bool(), operator ==() from the
//!  result of that lock() function. \n
//!   Basically this class implements multicast delegate; which provides multiple
//!  associations of different events from different providers. \n
//!   There are three methods provided to handle associated delegates. \n
//!   The first method is using the address of a function's static address. When
//!  deleting delegate, this method will delete all the associations that have
//!  equal function addresses, therefore not suitable for the situation that
//!  multiple objects are associated with the same method. \n
//!   The second method is using the reference of an object pointer.To use this
//!  method, an associated object should be managed through a kinda smart
//!  pointer, which provides the previously mentioned interface.When deleting a
//!  delegate with this method, it'll delete all the associations that have
//!  equal object reference, \n
//!   The third method is to use both.This can exactly erase the delegate that
//!  you want to delete. \n
//!   Since this class uses linear vector internally, and allocation is performed
//!  through appending last, multiple associations that are completely equal can
//!  be bound in one delegate. \n
//!   No specific alignment happens during appending or removing operation.Thus
//!  every lookup operation takes O(n) complexity. \n
//!   Removing delegate operation is queued in internal storage when the delegate
//!  is on execution to prevent data race. \n
//!   It does not assure callback execution order. \n
//! @{

//! @}
//! @}
} // namespace upp