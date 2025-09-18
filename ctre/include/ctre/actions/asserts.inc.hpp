#pragma once

#include "ctll/grammars.hpp"
#include "ctre/pcre_actions.hpp"

// push_assert_begin
template < auto V, typename... Ts, typename Parameters >
static constexpr auto apply(
    ctre::pcre::push_assert_begin,
    ctll::term< V >,
    ctre::pcre_context< ctll::list< Ts... >, Parameters > subject ) {
    return ctre::pcre_context{
        ctll::push_front( ctre::assert_line_begin(), subject.stack ),
        subject.parameters };
}

// push_assert_end
template < auto V, typename... Ts, typename Parameters >
static constexpr auto apply(
    ctre::pcre::push_assert_end,
    ctll::term< V >,
    ctre::pcre_context< ctll::list< Ts... >, Parameters > subject ) {
    return ctre::pcre_context{
        ctll::push_front( ctre::assert_line_end(), subject.stack ),
        subject.parameters };
}

// push_assert_begin
template < auto V, typename... Ts, typename Parameters >
static constexpr auto apply(
    ctre::pcre::push_assert_subject_begin,
    ctll::term< V >,
    ctre::pcre_context< ctll::list< Ts... >, Parameters > subject ) {
    return ctre::pcre_context{
        ctll::push_front( ctre::assert_subject_begin(), subject.stack ),
        subject.parameters };
}

// push_assert_subject_end
template < auto V, typename... Ts, typename Parameters >
static constexpr auto apply(
    ctre::pcre::push_assert_subject_end,
    ctll::term< V >,
    ctre::pcre_context< ctll::list< Ts... >, Parameters > subject ) {
    return ctre::pcre_context{
        ctll::push_front( ctre::assert_subject_end(), subject.stack ),
        subject.parameters };
}

// push_assert_subject_end_with_lineend
template < auto V, typename... Ts, typename Parameters >
static constexpr auto apply(
    ctre::pcre::push_assert_subject_end_with_lineend,
    ctll::term< V >,
    ctre::pcre_context< ctll::list< Ts... >, Parameters > subject ) {
    return ctre::pcre_context{
        ctll::push_front( ctre::assert_subject_end_line(), subject.stack ),
        subject.parameters };
}
