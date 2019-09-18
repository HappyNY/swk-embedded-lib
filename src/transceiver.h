#pragma once
#include <stdint.h>
#include <stdlib.h>

/*! \brief Enum type to indicate transceive result. This can hold normal integer values to indicate any number. 
    \details Return value under -8000 is regarded as implementation_specific error code. */
typedef enum {
    TRANSCEIVER_ZERO = 0,
    TRANSCEIVER_FAILED = -1,
    TRANSCEIVER_NO_CONNECTION = -2,
    TRANSCEIVER_TIMEOUT = -3,
    TRANSCEIVER_INVALID_DATA = -4,
    TRANSCEIVER_ALREADY_OPEN = -5,
    TRANSCEIVER_CLOSED = -6,
    //! \todo. Add more general error codes during developing process.

    TRANSCEIVER_IMPLEMENTATION_DOMAIN = -8000
} transceiver_result_t;

/*! \brief Function table to imitate the inheritance feature. */
struct transceiver_vtable
{
    //! \brief 

    //! \brief This function must return control right away with any result.
    //! \details If the connection is valid and there's nothing to read in the buffer or something, it should return zero to indicate that there's nothing to read.
    transceiver_result_t( *tryRead )( void* /*obj*/, char*/*rdbuf*/, size_t/*rdcnt*/ );
    
    //! \brief If the connection is valid, this function must wait until the data receive.
    transceiver_result_t( *read )( void* /*obj*/, char*/*rdbuf*/, size_t/*rdcnt*/ );
    
    //! \brief Tries write to transceiver. 
    //! \returns Number of bytes written to transmit buffer. Otherwise 0 or negative value to indicate the operation has failed.
    transceiver_result_t( *write ) ( void* /*obj*/, char const*/*wrbuf*/, size_t/*wrcnt*/ );
  
    //! \brief Notifies transceiver to try to connect. 
    //! \details To make the connection process begin, all required parameters must be supplied before calling this function. 
    transceiver_result_t( *open )( void* /*obj*/ );
};

//! Alias to easy use.
typedef struct transceiver_vtable transceiver_vtable_t;

/*! \brief The base type of all transceiver implementations.
    \details 
      This base type definition is recommended to be placed as the first member of any struct that wants to implement a transceiver interface. 
      Transceiver functions take transceiver_base_t instance as arguments, and virtual functions will be called by reinterpreting given arguments as the implementation struct that its value starts with given transceiver instance's address.
      Therefore, placing this base type at the mid of the other members can make the rest of the development process confusing.*/
typedef struct transceiver_vtable* tranceiver_base_t;

//! \todo. Determine function interface for callbacks.