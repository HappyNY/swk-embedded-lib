#include "algorithm.h"
#include "uassert.h"

size_t lowerbound(void const* arr, void const* eval, size_t elemSize, size_t numElems, int( *pred )( void const*, void const* ))
{
    uassert(arr && eval && pred && elemSize && numElems);

    size_t up = numElems, dn = 0;
    size_t idx = numElems >> 1;
    int ccomp, lcomp;

#define at(idx) ((char*)arr + ((idx)*elemSize))

    for ( ; idx;)
    {
        ccomp = pred(eval, at(idx));
        lcomp = pred(eval, at(idx - 1));

        if ( up - 1 <= dn )
        {
            idx += ccomp > 0;
            break;
        }

        if ( ccomp > 0 && lcomp > 0 )
            dn = idx;
        else if ( ccomp <= 0 && lcomp <= 0 )
            up = idx;
        else
            break;

        idx = ( dn + up ) >> 1;
    }

    return idx;
}
