#pragma once

#include "ctre/pcre_actions.hpp"

// backreference with name
template < auto... Str, auto V, typename... Ts, size_t Counter >
static constexpr auto apply(
    ctre::pcre::make_back_reference,
    ctll::term< V >,
    ctre::pcre_context< ctll::list< ctre::id< Str... >, Ts... >,
                        ctre::pcre_parameters< Counter > > ) {
    return ctre::pcre_context{
        ctll::push_front( back_reference_with_name< ctre::id< Str... > >(),
                          ctll::list< Ts... >() ),
        pcre_parameters< Counter >() };
}

// with just a number
template < auto V, size_t Id, typename... Ts, size_t Counter >
static constexpr auto apply(
    ctre::pcre::make_back_reference,
    ctll::term< V >,
    ctre::pcre_context< ctll::list< ctre::number< Id >, Ts... >,
                        ctre::pcre_parameters< Counter > > ) {
    // if we are looking outside of existing list of Ids ... reject input during
    // parsing
    if constexpr ( Counter < Id ) {
        return ctll::reject{};
    } else {
        return ctre::pcre_context{
            ctll::push_front( back_reference< Id >(), ctll::list< Ts... >() ),
            pcre_parameters< Counter >() };
    }
}

// relative backreference
template < auto V, size_t Id, typename... Ts, size_t Counter >
static constexpr auto apply( ctre::pcre::make_relative_back_reference,
                             ctll::term< V >,
                             [[maybe_unused]] ctre::pcre_context<
                                 ctll::list< ctre::number< Id >, Ts... >,
                                 ctre::pcre_parameters< Counter > > ) {
    // if we are looking outside of existing list of Ids ... reject input during
    // parsing
    if constexpr ( Counter < Id ) {
        return ctll::reject{};
    } else {
        constexpr size_t absolute_id = ( Counter + 1 ) - Id;
        return ctre::pcre_context{
            ctll::push_front( back_reference< absolute_id >(),
                              ctll::list< Ts... >() ),
            pcre_parameters< Counter >() };
    }
}
