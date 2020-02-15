#pragma once
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

//! @addtogroup uEmbedded_C
//! @{
//! @addtogroup uEmbedded_C_Algorithm
//! @{

/*! \brief      Find lowest index to insert [eval] parameter for sorted array.
    \note       [eval] and [arr] can be different type of object if you define
   predicate function in that way. Left side of predicate function will hold
   [eval]. */
size_t lowerbound(
  void const* arr,
  void const* eval,
  size_t      elemSize,
  size_t      numElems,
  int ( *pred )( void const* /*eval*/, void const* ) );

/*! \brief Insert new element to linear array
    \param arr          Array reference
    \param elem         Element reference. Set NULL to
    \param index        Index to insert new argument
    \param elemSize     Size of element
    \param lpNumElems   Number of element
    \returns Pointer to newly inserted item.
    \warning Maximum number of elements must be larger than numElems!
 */
void* array_insert(
  void const* arr,
  void const* elem,
  size_t      index,
  size_t      elemSize,
  size_t*     lpNumElems );

//! @brief      Change single byte value into two ASCII characters
static inline uint16_t byte_to_ascii( uint8_t const c )
{
    uint8_t const lo = c >> 4;
    uint8_t const hi = c & 0xf;

    uint8_t ch[2];
    ch[0] = ( lo ) + '0' + ( lo > 9 ) * ( 'a' - '0' );
    ch[1] = ( hi ) + '0' + ( hi > 9 ) * ( 'a' - '0' );

    return *(uint16_t*)ch;
}

//! @brief      Change two ASCII characters into single byte.
//! @returns    byte value between 0~255. Otherwise it's invalid ascii string
static inline int ascii_to_byte( void const* c )
{
    uint8_t hi = ( (uint8_t const*)c )[0];
    uint8_t lo = ( (uint8_t const*)c )[1];
    hi         = hi - '0' - ( 'a' - '0' ) * ( hi >= 'a' );
    lo         = lo - '0' - ( 'a' - '0' ) * ( lo >= 'a' );
    return ( hi << 4 ) + lo;
}

//! @}
//! @}
#ifdef __cplusplus
}
#endif