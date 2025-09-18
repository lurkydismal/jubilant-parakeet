#pragma once

#include "ctre/pcre_actions.hpp"

// push_word_boundary
template < auto V, typename... Ts, typename Parameters >
static constexpr auto apply(
    ctre::pcre::push_word_boundary,
    ctll::term< V >,
    ctre::pcre_context< ctll::list< Ts... >, Parameters > subject ) {
    return ctre::pcre_context{
        ctll::push_front( ctre::boundary< ctre::word_chars >(), subject.stack ),
        subject.parameters };
}

// push_not_word_boundary
template < auto V, typename... Ts, typename Parameters >
static constexpr auto apply(
    ctre::pcre::push_not_word_boundary,
    ctll::term< V >,
    ctre::pcre_context< ctll::list< Ts... >, Parameters > subject ) {
    return ctre::pcre_context{
        ctll::push_front(
            ctre::boundary< ctre::negative_set< ctre::word_chars > >(),
            subject.stack ),
        subject.parameters };
}
