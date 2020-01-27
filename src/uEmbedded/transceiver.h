#pragma once
#include <stdint.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

/*! \brief          Enum type to indicate transceive result. This can hold
   normal integer values to indicate any number. \details        Return value
   under -8000 is regarded as implementation_specific error code. */
typedef int32_t transceiver_result_t;

/*! \brief          Types of transceiver results. */
enum {
    TRANSCEIVER_ZERO          = 0,
    TRANSCEIVER_FAILED        = -1,
    TRANSCEIVER_NO_CONNECTION = -2,
    TRANSCEIVER_TIMEOUT       = -3,
    TRANSCEIVER_INVALID_DATA  = -4,
    TRANSCEIVER_ALREADY_OPEN  = -5,
    TRANSCEIVER_CLOSED        = -6,
    //! \todo. Add more general error codes during developing process.

    TRANSCEIVER_IMPLEMENTATION_DOMAIN = -8000
};

/*! \brief      Function table to imitate the inheritance feature. */
struct transceiver_vtable {
    //! \brief      If the connection is valid, this function must wait until
    //! the data receive.
    transceiver_result_t ( *read )( void* /*obj*/, char* /*rdbuf*/,
                                    size_t /*rdcnt*/ );

    //! \brief      Tries write to transceiver.
    //! \returns    Number of bytes written to transmit buffer. Otherwise 0 or
    //! negative value to indicate the operation has failed.
    transceiver_result_t ( *write )( void* /*obj*/, char const* /*wrbuf*/,
                                     size_t /*wrcnt*/ );

    //! Controls IO
    transceiver_result_t ( *ioctl )( void* /*obj*/, intptr_t /*cmd*/ );

    //! Controls IO
    transceiver_result_t ( *close )( void* /*obj*/ );
};

//! Alias to easy use.
typedef struct transceiver_vtable transceiver_vtable_t;

//! Actual transceiver desriptor type
typedef struct tranceiver_desc {
    transceiver_vtable_t* vt_;
}* tr_desc_t__;

/*! \brief      The base type of all transceiver implementations.
    \details
      This base type definition is recommended to be placed as the first member
   of any struct that wants to implement a transceiver interface. Transceiver
   functions take transceiver_base_t instance as arguments, and virtual
   functions will be called by reinterpreting given arguments as the
   implementation struct that its value starts with given transceiver instance's
   address. Therefore, placing this base type at the mid of the other members
   can make the rest of the development process confusing.*/
typedef intptr_t transceiver_descriptor_t;

//! Read data from the transceiver.
static inline transceiver_result_t td_read( transceiver_descriptor_t desc, char* buf, size_t rdcnt )
{
    transceiver_vtable_t* td = ((tr_desc_t__)desc)->vt_;
    return td->read( (void*)desc, buf, rdcnt );
}

//! Write data into the transceiver
static inline transceiver_result_t td_write( transceiver_descriptor_t desc, char* buf, size_t wrcnt )
{
    transceiver_vtable_t* td = ( (tr_desc_t__) desc )->vt_;
    return td->write( (void*)td, buf, wrcnt );
}

//! Deliever control command to transceiver
static inline transceiver_result_t td_ioctl( transceiver_descriptor_t desc, intptr_t cmd )
{
    transceiver_vtable_t* td = ( (tr_desc_t__) desc )->vt_;
    return td->ioctl( td, cmd );
}

//! Close transceiver
static inline transceiver_result_t td_close( transceiver_descriptor_t desc )
{
    transceiver_vtable_t* td = ( (tr_desc_t__) desc )->vt_;
    return td->close( td );
}
#ifdef __cplusplus
}
#endif