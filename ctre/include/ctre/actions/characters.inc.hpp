#pragma once

#include "ctre/pcre_actions.hpp"

// push character
template < auto V, typename... Ts, typename Parameters >
static constexpr auto apply(
    ctre::pcre::push_character,
    ctll::term< V >,
    ctre::pcre_context< ctll::list< Ts... >, Parameters > subject ) {
    return ctre::pcre_context{
        ctll::push_front( character< V >(), subject.stack ),
        subject.parameters };
}
// push_any_character
template < auto V, typename... Ts, typename Parameters >
static constexpr auto apply(
    ctre::pcre::push_character_anything,
    ctll::term< V >,
    ctre::pcre_context< ctll::list< Ts... >, Parameters > subject ) {
    return ctre::pcre_context{ ctll::push_front( ctre::any(), subject.stack ),
                               subject.parameters };
}
// character_alarm
template < auto V, typename... Ts, typename Parameters >
static constexpr auto apply(
    ctre::pcre::push_character_alarm,
    ctll::term< V >,
    ctre::pcre_context< ctll::list< Ts... >, Parameters > subject ) {
    return ctre::pcre_context{
        ctll::push_front( ctre::character< '\x07' >(), subject.stack ),
        subject.parameters };
}
// character_escape
template < auto V, typename... Ts, typename Parameters >
static constexpr auto apply(
    ctre::pcre::push_character_escape,
    ctll::term< V >,
    ctre::pcre_context< ctll::list< Ts... >, Parameters > subject ) {
    return ctre::pcre_context{
        ctll::push_front( ctre::character< '\x14' >(), subject.stack ),
        subject.parameters };
}
// character_formfeed
template < auto V, typename... Ts, typename Parameters >
static constexpr auto apply(
    ctre::pcre::push_character_formfeed,
    ctll::term< V >,
    ctre::pcre_context< ctll::list< Ts... >, Parameters > subject ) {
    return ctre::pcre_context{
        ctll::push_front( ctre::character< '\x0C' >(), subject.stack ),
        subject.parameters };
}
// push_character_newline
template < auto V, typename... Ts, typename Parameters >
static constexpr auto apply(
    ctre::pcre::push_character_newline,
    ctll::term< V >,
    ctre::pcre_context< ctll::list< Ts... >, Parameters > subject ) {
    return ctre::pcre_context{
        ctll::push_front( ctre::character< '\x0A' >(), subject.stack ),
        subject.parameters };
}
// push_character_null
template < auto V, typename... Ts, typename Parameters >
static constexpr auto apply(
    ctre::pcre::push_character_null,
    ctll::term< V >,
    ctre::pcre_context< ctll::list< Ts... >, Parameters > subject ) {
    return ctre::pcre_context{
        ctll::push_front( ctre::character< '\0' >(), subject.stack ),
        subject.parameters };
}
// push_character_return_carriage
template < auto V, typename... Ts, typename Parameters >
static constexpr auto apply(
    ctre::pcre::push_character_return_carriage,
    ctll::term< V >,
    ctre::pcre_context< ctll::list< Ts... >, Parameters > subject ) {
    return ctre::pcre_context{
        ctll::push_front( ctre::character< '\x0D' >(), subject.stack ),
        subject.parameters };
}
// push_character_tab
template < auto V, typename... Ts, typename Parameters >
static constexpr auto apply(
    ctre::pcre::push_character_tab,
    ctll::term< V >,
    ctre::pcre_context< ctll::list< Ts... >, Parameters > subject ) {
    return ctre::pcre_context{
        ctll::push_front( ctre::character< '\x09' >(), subject.stack ),
        subject.parameters };
}
