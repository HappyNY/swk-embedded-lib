#pragma once

#if __cplusplus < 201700L
#    include <experimental/optional>
#else
#    include <optional>
#endif

#include <stdint.h>
namespace upp {
namespace hash {

// FNV1a c++11 constexpr compile time hash functions, 32 and 64 bit
// str should be a null terminated string literal, value should be left out
// e.g hash_32_fnv1a_const("example")
// code license: public domain or equivalent
// post: https://notes.underscorediscovery.com/constexpr-fnv1a/

constexpr uint32_t val_32_const   = 0x811c9dc5;
constexpr uint32_t prime_32_const = 0x1000193;
constexpr uint64_t val_64_const   = 0xcbf29ce484222325;
constexpr uint64_t prime_64_const = 0x100000001b3;

inline constexpr uint32_t fnv1a_32_const(
    const char* const str,
    const uint32_t    value = val_32_const ) noexcept
{
    return ( str[0] == '\0' )
               ? value
               : fnv1a_32_const(
                   &str[1],
                   ( value ^ uint32_t( str[0] ) ) * prime_32_const );
}

inline constexpr uint64_t
fnv1a_64( const char* const str, const uint64_t value = val_64_const ) noexcept
{
    return ( str[0] == '\0' )
               ? value
               : fnv1a_64(
                   &str[1],
                   ( value ^ uint64_t( str[0] ) ) * prime_64_const );
}

inline const uint32_t fnv1a_32( const void* key )
{
    const char* data  = (char*)key;
    uint32_t    hash  = 0x811c9dc5;
    uint32_t    prime = 0x1000193;

    for ( ; *data; ++data ) {
        hash = hash ^ *data;
        hash *= prime;
    }

    return hash;

} // hash_32_fnv1a

} // namespace hash

#if __cplusplus < 201700L
template <class ty__>
using optional = std::experimental::optional<ty__>;
#else
template <class ty__>
using optional = std::optional<ty__>;
#endif

} // namespace upp