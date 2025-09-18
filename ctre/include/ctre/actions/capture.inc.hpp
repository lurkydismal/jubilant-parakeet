#pragma once

#include "ctre/pcre_actions.hpp"

// prepare_capture
template < auto V, typename... Ts, size_t Counter >
static constexpr auto apply(
    ctre::pcre::prepare_capture,
    ctll::term< V >,
    ctre::pcre_context< ctll::list< Ts... >,
                        ctre::pcre_parameters< Counter > > ) {
    return ctre::pcre_context{
        ctll::push_front( capture_id< Counter + 1 >(), ctll::list< Ts... >() ),
        pcre_parameters< Counter + 1 >() };
}

// reset_capture
template < auto V, typename... Ts, size_t Id, size_t Counter >
static constexpr auto apply(
    ctre::pcre::reset_capture,
    ctll::term< V >,
    ctre::pcre_context< ctll::list< ctre::capture_id< Id >, Ts... >,
                        ctre::pcre_parameters< Counter > > ) {
    return ctre::pcre_context{ ctll::list< Ts... >(),
                               pcre_parameters< Counter - 1 >() };
}

// capture
template < auto V, typename A, size_t Id, typename... Ts, size_t Counter >
static constexpr auto apply(
    ctre::pcre::make_capture,
    ctll::term< V >,
    ctre::pcre_context< ctll::list< A, ctre::capture_id< Id >, Ts... >,
                        ctre::pcre_parameters< Counter > > ) {
    return ctre::pcre_context{
        ctll::push_front( capture< Id, A >(), ctll::list< Ts... >() ),
        pcre_parameters< Counter >() };
}
// capture (sequence)
template < auto V,
           typename... Content,
           size_t Id,
           typename... Ts,
           size_t Counter >
static constexpr auto apply(
    ctre::pcre::make_capture,
    ctll::term< V >,
    ctre::pcre_context< ctll::list< ctre::sequence< Content... >,
                                    ctre::capture_id< Id >,
                                    Ts... >,
                        ctre::pcre_parameters< Counter > > ) {
    return ctre::pcre_context{
        ctll::push_front( capture< Id, Content... >(), ctll::list< Ts... >() ),
        pcre_parameters< Counter >() };
}
// push_name
template < auto V, typename... Ts, typename Parameters >
static constexpr auto apply(
    ctre::pcre::push_name,
    ctll::term< V >,
    ctre::pcre_context< ctll::list< Ts... >, Parameters > subject ) {
    return ctre::pcre_context{ ctll::push_front( id< V >(), subject.stack ),
                               subject.parameters };
}
// push_name (concat)
template < auto... Str, auto V, typename... Ts, typename Parameters >
static constexpr auto apply(
    ctre::pcre::push_name,
    ctll::term< V >,
    ctre::pcre_context< ctll::list< ctre::id< Str... >, Ts... >, Parameters >
        subject ) {
    return ctre::pcre_context{
        ctll::push_front( id< Str..., V >(), ctll::list< Ts... >() ),
        subject.parameters };
}
// capture with name
template < auto... Str,
           auto V,
           typename A,
           size_t Id,
           typename... Ts,
           size_t Counter >
static constexpr auto apply(
    ctre::pcre::make_capture_with_name,
    ctll::term< V >,
    ctre::pcre_context<
        ctll::list< A, ctre::id< Str... >, ctre::capture_id< Id >, Ts... >,
        ctre::pcre_parameters< Counter > > ) {
    return ctre::pcre_context{
        ctll::push_front( capture_with_name< Id, ctre::id< Str... >, A >(),
                          ctll::list< Ts... >() ),
        pcre_parameters< Counter >() };
}
// capture with name (sequence)
template < auto... Str,
           auto V,
           typename... Content,
           size_t Id,
           typename... Ts,
           size_t Counter >
static constexpr auto apply(
    ctre::pcre::make_capture_with_name,
    ctll::term< V >,
    ctre::pcre_context< ctll::list< ctre::sequence< Content... >,
                                    ctre::id< Str... >,
                                    ctre::capture_id< Id >,
                                    Ts... >,
                        ctre::pcre_parameters< Counter > > ) {
    return ctre::pcre_context{
        ctll::push_front(
            capture_with_name< Id, ctre::id< Str... >, Content... >(),
            ctll::list< Ts... >() ),
        pcre_parameters< Counter >() };
}
