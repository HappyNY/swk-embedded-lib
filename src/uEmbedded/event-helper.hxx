#pragma once
#include <tuple>
#include "event-procedure.h"
// ----------UNPACK TUPLE AND APPLY TO FUNCTION ---------

using namespace std;

// ------------- UTILITY---------------
template <int...>
struct index_tuple
{ };

template <int I, typename IndexTuple, typename... Types>
struct make_indexes_impl;

template <int I, int... Indexes, typename T, typename... Types>
struct make_indexes_impl<I, index_tuple<Indexes...>, T, Types...>
{
    typedef typename make_indexes_impl<I + 1, index_tuple<Indexes..., I>, Types...>::type type;
};

template <int I, int... Indexes>
struct make_indexes_impl<I, index_tuple<Indexes...>>
{
    typedef index_tuple<Indexes...> type;
};

template <typename... Types>
struct make_indexes : make_indexes_impl<0, index_tuple<>, Types...>
{ };

template <class Ret, class... Args, int... Indexes>
Ret apply_helper( Ret ( *pf )( Args... ), index_tuple<Indexes...>, tuple<Args...>&& tup )
{
    return pf( forward<Args>( get<Indexes>( tup ) )... );
}

template <class Ret, class... Args>
Ret apply( Ret ( *pf )( Args... ), const tuple<Args...>& tup )
{
    return apply_helper( pf, typename make_indexes<Args...>::type(), tuple<Args...>( tup ) );
}

template <class Ret, class... Args>
Ret apply( Ret ( *pf )( Args... ), tuple<Args...>&& tup )
{
    return apply_helper( pf, typename make_indexes<Args...>::type(), forward<tuple<Args...>>( tup ) );
}

template <typename _fnc, typename... _args>
inline void QueueEventHelper( EventQueue* queue, _fnc func, _args... args )
{
    using pack_t = std::tuple<_args...>;

    struct bundle_t
    {
        _fnc   func;
        pack_t pack;
    } bundle = { func, std::make_tuple( std::forward<_args>( args )... ) };

    EventCallbackType lambda = []( void* param ) {
        auto pb = (bundle_t*)param;
        apply( pb->func, pb->pack );
    };

    QueueEvent( queue, lambda, (void const*)&bundle, sizeof( bundle ) );
}

template <>
inline void QueueEventHelper( EventQueue* queue, void ( *fnc )( void ) )
{
    EventCallbackType cb = []( void* v ) { reinterpret_cast<decltype( fnc )>( v )(); };
    QueueEvent( queue, cb, (void const*)fnc, sizeof( fnc ) );
}
