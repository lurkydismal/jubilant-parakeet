#pragma once

#include "ctre/pcre_actions.hpp"

// lookahead positive start
template < auto V, typename... Ts, size_t Counter >
static constexpr auto apply(
    ctre::pcre::start_lookahead_positive,
    ctll::term< V >,
    ctre::pcre_context< ctll::list< Ts... >,
                        ctre::pcre_parameters< Counter > > ) {
    return ctre::pcre_context{
        ctll::list< ctre::look_start< ctre::lookahead_positive<> >, Ts... >(),
        pcre_parameters< Counter >() };
}

// lookahead positive end
template < auto V, typename Look, typename... Ts, size_t Counter >
static constexpr auto apply(
    ctre::pcre::look_finish,
    ctll::term< V >,
    ctre::pcre_context<
        ctll::
            list< Look, ctre::look_start< ctre::lookahead_positive<> >, Ts... >,
        ctre::pcre_parameters< Counter > > ) {
    return ctre::pcre_context{
        ctll::list< ctre::lookahead_positive< Look >, Ts... >(),
        pcre_parameters< Counter >() };
}

// lookahead positive end (sequence)
template < auto V, typename... Look, typename... Ts, size_t Counter >
static constexpr auto apply(
    ctre::pcre::look_finish,
    ctll::term< V >,
    ctre::pcre_context<
        ctll::list< ctre::sequence< Look... >,
                    ctre::look_start< ctre::lookahead_positive<> >,
                    Ts... >,
        ctre::pcre_parameters< Counter > > ) {
    return ctre::pcre_context{
        ctll::list< ctre::lookahead_positive< Look... >, Ts... >(),
        pcre_parameters< Counter >() };
}

// lookahead negative start
template < auto V, typename... Ts, size_t Counter >
static constexpr auto apply(
    ctre::pcre::start_lookahead_negative,
    ctll::term< V >,
    ctre::pcre_context< ctll::list< Ts... >,
                        ctre::pcre_parameters< Counter > > ) {
    return ctre::pcre_context{
        ctll::list< ctre::look_start< ctre::lookahead_negative<> >, Ts... >(),
        pcre_parameters< Counter >() };
}

// lookahead negative end
template < auto V, typename Look, typename... Ts, size_t Counter >
static constexpr auto apply(
    ctre::pcre::look_finish,
    ctll::term< V >,
    ctre::pcre_context<
        ctll::
            list< Look, ctre::look_start< ctre::lookahead_negative<> >, Ts... >,
        ctre::pcre_parameters< Counter > > ) {
    return ctre::pcre_context{
        ctll::list< ctre::lookahead_negative< Look >, Ts... >(),
        pcre_parameters< Counter >() };
}

// lookahead negative end (sequence)
template < auto V, typename... Look, typename... Ts, size_t Counter >
static constexpr auto apply(
    ctre::pcre::look_finish,
    ctll::term< V >,
    ctre::pcre_context<
        ctll::list< ctre::sequence< Look... >,
                    ctre::look_start< ctre::lookahead_negative<> >,
                    Ts... >,
        ctre::pcre_parameters< Counter > > ) {
    return ctre::pcre_context{
        ctll::list< ctre::lookahead_negative< Look... >, Ts... >(),
        pcre_parameters< Counter >() };
}

// LOOKBEHIND

// lookbehind positive start
template < auto V, typename... Ts, size_t Counter >
static constexpr auto apply(
    ctre::pcre::start_lookbehind_positive,
    ctll::term< V >,
    ctre::pcre_context< ctll::list< Ts... >,
                        ctre::pcre_parameters< Counter > > ) {
    return ctre::pcre_context{
        ctll::list< ctre::look_start< ctre::lookbehind_positive<> >, Ts... >(),
        pcre_parameters< Counter >() };
}

// lookbehind positive end
template < auto V, typename Look, typename... Ts, size_t Counter >
static constexpr auto apply(
    ctre::pcre::look_finish,
    ctll::term< V >,
    ctre::pcre_context<
        ctll::list< Look,
                    ctre::look_start< ctre::lookbehind_positive<> >,
                    Ts... >,
        ctre::pcre_parameters< Counter > > ) {
    return ctre::pcre_context{
        ctll::list<
            ctre::lookbehind_positive<
                decltype( ctre::rotate_for_lookbehind::rotate( Look{} ) ) >,
            Ts... >(),
        pcre_parameters< Counter >() };
}

// lookbehind positive end (sequence)
template < auto V, typename... Look, typename... Ts, size_t Counter >
static constexpr auto apply(
    ctre::pcre::look_finish,
    ctll::term< V >,
    ctre::pcre_context<
        ctll::list< ctre::sequence< Look... >,
                    ctre::look_start< ctre::lookbehind_positive<> >,
                    Ts... >,
        ctre::pcre_parameters< Counter > > ) {
    using my_lookbehind =
        decltype( ctre::convert_to_basic_list< ctre::lookbehind_positive >(
            ctll::rotate(
                ctll::list< decltype( ctre::rotate_for_lookbehind::rotate(
                    Look{} ) )... >{} ) ) );
    return ctre::pcre_context{ ctll::list< my_lookbehind, Ts... >(),
                               pcre_parameters< Counter >() };
}

// lookbehind negative start
template < auto V, typename... Ts, size_t Counter >
static constexpr auto apply(
    ctre::pcre::start_lookbehind_negative,
    ctll::term< V >,
    ctre::pcre_context< ctll::list< Ts... >,
                        ctre::pcre_parameters< Counter > > ) {
    return ctre::pcre_context{
        ctll::list< ctre::look_start< ctre::lookbehind_negative<> >, Ts... >(),
        pcre_parameters< Counter >() };
}

// lookbehind negative end
template < auto V, typename Look, typename... Ts, size_t Counter >
static constexpr auto apply(
    ctre::pcre::look_finish,
    ctll::term< V >,
    ctre::pcre_context<
        ctll::list< Look,
                    ctre::look_start< ctre::lookbehind_negative<> >,
                    Ts... >,
        ctre::pcre_parameters< Counter > > ) {
    return ctre::pcre_context{
        ctll::list<
            ctre::lookbehind_negative<
                decltype( ctre::rotate_for_lookbehind::rotate( Look{} ) ) >,
            Ts... >(),
        pcre_parameters< Counter >() };
}

// lookbehind negative end (sequence)
template < auto V, typename... Look, typename... Ts, size_t Counter >
static constexpr auto apply(
    ctre::pcre::look_finish,
    ctll::term< V >,
    ctre::pcre_context<
        ctll::list< ctre::sequence< Look... >,
                    ctre::look_start< ctre::lookbehind_negative<> >,
                    Ts... >,
        ctre::pcre_parameters< Counter > > ) {
    using my_lookbehind =
        decltype( ctre::convert_to_basic_list< ctre::lookbehind_negative >(
            ctll::rotate(
                ctll::list< decltype( ctre::rotate_for_lookbehind::rotate(
                    Look{} ) )... >{} ) ) );
    return ctre::pcre_context{ ctll::list< my_lookbehind, Ts... >(),
                               pcre_parameters< Counter >() };
}
