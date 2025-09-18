#pragma once

#include "ctre/pcre_actions.hpp"

// atomic start
template < auto V, typename... Ts, size_t Counter >
static constexpr auto apply(
    ctre::pcre::start_atomic,
    ctll::term< V >,
    ctre::pcre_context< ctll::list< Ts... >,
                        ctre::pcre_parameters< Counter > > ) {
    return ctre::pcre_context{ ctll::list< ctre::atomic_start, Ts... >(),
                               pcre_parameters< Counter >() };
}

// atomic
template < auto V, typename Atomic, typename... Ts, size_t Counter >
static constexpr auto apply(
    ctre::pcre::make_atomic,
    ctll::term< V >,
    ctre::pcre_context< ctll::list< Atomic, ctre::atomic_start, Ts... >,
                        ctre::pcre_parameters< Counter > > ) {
    return ctre::pcre_context{
        ctll::list< ctre::atomic_group< Atomic >, Ts... >(),
        pcre_parameters< Counter >() };
}

// atomic sequence
template < auto V, typename... Atomic, typename... Ts, size_t Counter >
static constexpr auto apply(
    ctre::pcre::make_atomic,
    ctll::term< V >,
    ctre::pcre_context<
        ctll::list< ctre::sequence< Atomic... >, ctre::atomic_start, Ts... >,
        ctre::pcre_parameters< Counter > > ) {
    return ctre::pcre_context{
        ctll::list< ctre::atomic_group< Atomic... >, Ts... >(),
        pcre_parameters< Counter >() };
}
