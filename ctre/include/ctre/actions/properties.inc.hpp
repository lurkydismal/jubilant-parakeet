#pragma once

#include "ctre/pcre_actions.hpp"

// push_property_name
template < auto V, typename... Ts, typename Parameters >
static constexpr auto apply(
    ctre::pcre::push_property_name,
    ctll::term< V >,
    ctre::pcre_context< ctll::list< Ts... >, Parameters > subject ) {
    return ctre::pcre_context{
        ctll::push_front( property_name< V >(), subject.stack ),
        subject.parameters };
}
// push_property_name (concat)
template < auto... Str, auto V, typename... Ts, typename Parameters >
static constexpr auto apply(
    ctre::pcre::push_property_name,
    ctll::term< V >,
    ctre::pcre_context< ctll::list< ctre::property_name< Str... >, Ts... >,
                        Parameters > subject ) {
    return ctre::pcre_context{
        ctll::push_front( property_name< Str..., V >(), ctll::list< Ts... >() ),
        subject.parameters };
}

// push_property_value
template < auto V, typename... Ts, typename Parameters >
static constexpr auto apply(
    ctre::pcre::push_property_value,
    ctll::term< V >,
    ctre::pcre_context< ctll::list< Ts... >, Parameters > subject ) {
    return ctre::pcre_context{
        ctll::push_front( property_value< V >(), subject.stack ),
        subject.parameters };
}
// push_property_value (concat)
template < auto... Str, auto V, typename... Ts, typename Parameters >
static constexpr auto apply(
    ctre::pcre::push_property_value,
    ctll::term< V >,
    ctre::pcre_context< ctll::list< ctre::property_value< Str... >, Ts... >,
                        Parameters > subject ) {
    return ctre::pcre_context{ ctll::push_front( property_value< Str..., V >(),
                                                 ctll::list< Ts... >() ),
                               subject.parameters };
}

// make_property
template < auto V, auto... Name, typename... Ts, typename Parameters >
static constexpr auto apply(
    ctre::pcre::make_property,
    ctll::term< V >,
    [[maybe_unused]] ctre::pcre_context<
        ctll::list< ctre::property_name< Name... >, Ts... >,
        Parameters > subject ) {
    // return ctll::reject{};
    constexpr char name[ sizeof...( Name ) ]{ static_cast< char >( Name )... };
    constexpr auto p =
        uni::detail::binary_prop_from_string( get_string_view( name ) );

    if constexpr ( uni::detail::is_unknown( p ) ) {
        return ctll::reject{};
    } else {
        return ctre::pcre_context{
            ctll::push_front( make_binary_property< p >(),
                              ctll::list< Ts... >() ),
            subject.parameters };
    }
}

// make_property
template < auto V,
           auto... Value,
           auto... Name,
           typename... Ts,
           typename Parameters >
static constexpr auto apply( ctre::pcre::make_property,
                             ctll::term< V >,
                             [[maybe_unused]] ctre::pcre_context<
                                 ctll::list< ctre::property_value< Value... >,
                                             ctre::property_name< Name... >,
                                             Ts... >,
                                 Parameters > subject ) {
    // return ctll::reject{};
    constexpr auto prop =
        ctre::property_builder< Name... >::template get< Value... >();

    if constexpr ( std::is_same_v< decltype( prop ), ctll::reject > ) {
        return ctll::reject{};
    } else {
        return ctre::pcre_context{
            ctll::push_front( prop, ctll::list< Ts... >() ),
            subject.parameters };
    }
}

// make_property_negative
template < auto V, auto... Name, typename... Ts, typename Parameters >
static constexpr auto apply(
    ctre::pcre::make_property_negative,
    ctll::term< V >,
    [[maybe_unused]] ctre::pcre_context<
        ctll::list< ctre::property_name< Name... >, Ts... >,
        Parameters > subject ) {
    // return ctll::reject{};
    constexpr char name[ sizeof...( Name ) ]{ static_cast< char >( Name )... };
    constexpr auto p =
        uni::detail::binary_prop_from_string( get_string_view( name ) );

    if constexpr ( uni::detail::is_unknown( p ) ) {
        return ctll::reject{};
    } else {
        return ctre::pcre_context{
            ctll::push_front( negate< ctre::make_binary_property< p > >(),
                              ctll::list< Ts... >() ),
            subject.parameters };
    }
}

// make_property_negative
template < auto V,
           auto... Value,
           auto... Name,
           typename... Ts,
           typename Parameters >
static constexpr auto apply( ctre::pcre::make_property_negative,
                             ctll::term< V >,
                             [[maybe_unused]] ctre::pcre_context<
                                 ctll::list< ctre::property_value< Value... >,
                                             ctre::property_name< Name... >,
                                             Ts... >,
                                 Parameters > subject ) {
    // return ctll::reject{};
    constexpr auto prop =
        ctre::property_builder< Name... >::template get< Value... >();

    if constexpr ( std::is_same_v< decltype( prop ), ctll::reject > ) {
        return ctll::reject{};
    } else {
        return ctre::pcre_context{
            ctll::push_front( negate< decltype( prop ) >(),
                              ctll::list< Ts... >() ),
            subject.parameters };
    }
}
