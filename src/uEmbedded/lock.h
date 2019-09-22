#pragma once

struct uemb_lock
{
    void lock( void* );
    void unlock( void* );
    void* object;
};