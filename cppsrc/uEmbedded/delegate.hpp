//! @brief Delegate
//! @file
//! @author Seungwoo Kang (ki6080@gmail.com)
//! @copyright Copyright (c) 2019. Seungwoo Kang. All rights reserved.
//!
//! @details
//!  A delegate class that supports any pointer class that provides the
//!  following interface; lock(), and operator bool(), operator ==() from the
//!  result of that lock() function.
//!
//!  Basically this class implements multicast delegate; which provides multiple
//!  associations of different events from different providers.
//!
//!  There are three methods provided to handle associated delegates.
//!
//!  The first method is using the address of a function's static address. When
//!  deleting delegate, this method will delete all the associations that have
//!  equal function addresses, therefore not suitable for the situation that
//!  multiple objects are associated with the same method.
//!
//!  The second method is using the reference of an object pointer.To use this
//!  method, an associated object should be managed through a kinda smart
//!  pointer, which provides the previously mentioned interface.When deleting a
//!  delegate with this method, it'll delete all the associations that have
//!  equal object reference,
//!
//!  The third method is to use both.This can exactly erase the delegate that
//!  you want to delete.
//!
//!  Since this class uses linear vector internally, and allocation is performed
//!  through appending last, multiple associations that are completely equal can
//!  be bound in one delegate.
//!
//!  No specific alignment happens during appending or removing operation.Thus
//!  every lookup operation takes O(n) complexity.
//!
//!  Removing delegate operation is queued in internal storage when the delegate
//!  is on execution to prevent data race.
//!
//!  It does not assure callback execution order.
#pragma once

