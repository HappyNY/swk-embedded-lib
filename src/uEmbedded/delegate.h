#pragma once
#include "managed_reference_pool.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct delegate delegate_t;

typedef void (
    *delegate_event_cb_t )( refhandle_t const* obj, void* event_args );

void delegate_assign(
    delegate_t*         s,
    refhandle_t const*  objref,
    delegate_event_cb_t callback );
void   delegate_clear( delegate_t* s );
size_t delegate_size( delegate_t* s );
size_t delegate_delete(
    delegate_t*         s,
    refhandle_t const*  objref,
    delegate_event_cb_t callback );
void        delegate_call( delegate_t* s, void* event_args );
void        delegate_destroy( delegate_t* s );
delegate_t* delegate_create( bool bMulticast );

#ifdef __cplusplus
}
#endif