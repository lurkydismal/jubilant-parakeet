#pragma once

#include "ctre/pcre_actions.hpp"

// we need to reset counter and wrap Mode into mode_switch
template < typename Mode, typename... Ts, typename Parameters >
static constexpr auto apply_mode( Mode, ctll::list< Ts... >, Parameters ) {
    return ctre::pcre_context< ctll::list< ctre::mode_switch< Mode >, Ts... >,
                               Parameters >{};
}

template < typename Mode, typename... Ts, size_t Id, size_t Counter >
static constexpr auto apply_mode( Mode,
                                  ctll::list< ctre::capture_id< Id >, Ts... >,
                                  ctre::pcre_parameters< Counter > ) {
    return ctre::pcre_context< ctll::list< ctre::mode_switch< Mode >, Ts... >,
                               ctre::pcre_parameters< Counter - 1 > >{};
}

// catch a semantic action into mode
template < auto V, typename... Ts, typename Parameters >
static constexpr auto apply(
    ctre::pcre::mode_case_insensitive mode,
    ctll::term< V >,
    ctre::pcre_context< ctll::list< Ts... >, Parameters > ) {
    return apply_mode( mode, ctll::list< Ts... >{}, Parameters{} );
}

template < auto V, typename... Ts, typename Parameters >
static constexpr auto apply(
    ctre::pcre::mode_case_sensitive mode,
    ctll::term< V >,
    ctre::pcre_context< ctll::list< Ts... >, Parameters > ) {
    return apply_mode( mode, ctll::list< Ts... >{}, Parameters{} );
}

template < auto V, typename... Ts, typename Parameters >
static constexpr auto apply(
    ctre::pcre::mode_singleline mode,
    ctll::term< V >,
    ctre::pcre_context< ctll::list< Ts... >, Parameters > ) {
    return apply_mode( mode, ctll::list< Ts... >{}, Parameters{} );
}

template < auto V, typename... Ts, typename Parameters >
static constexpr auto apply(
    ctre::pcre::mode_multiline mode,
    ctll::term< V >,
    ctre::pcre_context< ctll::list< Ts... >, Parameters > ) {
    return apply_mode( mode, ctll::list< Ts... >{}, Parameters{} );
}

// to properly reset capture
