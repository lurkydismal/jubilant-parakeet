#pragma once

#include "ctre/atoms.hpp"
#include "ctre/pcre_actions.hpp"

// make_sequence
template < auto V, typename A, typename B, typename... Ts, typename Parameters >
static constexpr auto apply(
    ctre::pcre::make_sequence,
    ctll::term< V >,
    ctre::pcre_context< ctll::list< B, A, Ts... >, Parameters > subject ) {
    return ctre::pcre_context{
        ctll::push_front( sequence< A, B >(), ctll::list< Ts... >() ),
        subject.parameters };
}
// make_sequence (concat)
template < auto V,
           typename A,
           typename... Bs,
           typename... Ts,
           typename Parameters >
static constexpr auto apply(
    ctre::pcre::make_sequence,
    ctll::term< V >,
    ctre::pcre_context< ctll::list< ctre::sequence< Bs... >, A, Ts... >,
                        Parameters > subject ) {
    return ctre::pcre_context{
        ctll::push_front( sequence< A, Bs... >(), ctll::list< Ts... >() ),
        subject.parameters };
}

// make_sequence (make string)
template < auto V, auto A, auto B, typename... Ts, typename Parameters >
static constexpr auto apply(
    ctre::pcre::make_sequence,
    ctll::term< V >,
    ctre::pcre_context<
        ctll::list< ctre::character< B >, ctre::character< A >, Ts... >,
        Parameters > subject ) {
    return ctre::pcre_context{
        ctll::push_front( string< A, B >(), ctll::list< Ts... >() ),
        subject.parameters };
}
// make_sequence (concat string)
template < auto V, auto A, auto... Bs, typename... Ts, typename Parameters >
static constexpr auto apply(
    ctre::pcre::make_sequence,
    ctll::term< V >,
    ctre::pcre_context<
        ctll::list< ctre::string< Bs... >, ctre::character< A >, Ts... >,
        Parameters > subject ) {
    return ctre::pcre_context{
        ctll::push_front( string< A, Bs... >(), ctll::list< Ts... >() ),
        subject.parameters };
}

// make_sequence (make string in front of different items)
template < auto V,
           auto A,
           auto B,
           typename... Sq,
           typename... Ts,
           typename Parameters >
static constexpr auto apply(
    ctre::pcre::make_sequence,
    ctll::term< V >,
    ctre::pcre_context<
        ctll::list< ctre::sequence< ctre::character< B >, Sq... >,
                    ctre::character< A >,
                    Ts... >,
        Parameters > subject ) {
    return ctre::pcre_context{
        ctll::push_front( sequence< ctre::string< A, B >, Sq... >(),
                          ctll::list< Ts... >() ),
        subject.parameters };
}
// make_sequence (concat string in front of different items)
template < auto V,
           auto A,
           auto... Bs,
           typename... Sq,
           typename... Ts,
           typename Parameters >
static constexpr auto apply(
    ctre::pcre::make_sequence,
    ctll::term< V >,
    ctre::pcre_context<
        ctll::list< ctre::sequence< ctre::string< Bs... >, Sq... >,
                    ctre::character< A >,
                    Ts... >,
        Parameters > subject ) {
    return ctre::pcre_context{
        ctll::push_front( sequence< ctre::string< A, Bs... >, Sq... >(),
                          ctll::list< Ts... >() ),
        subject.parameters };
}
