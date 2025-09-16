#pragma once

#include <format>
#include <gsl/gsl>
#include <span>
#include <string_view>

namespace arhodigp {

using state_t = gsl::not_null< void* >;

auto parseArguments( std::string& _format,
                     std::span< std::string_view > _arguments,
                     std::string_view _applicationIdentifier,
                     std::string_view _applicationDescription,
                     float _applicationVersion,
                     std::string_view _contactAddress ) -> bool;

auto parseArguments( std::string_view _format,
                     std::span< std::string_view > _arguments,
                     std::string_view _applicationIdentifier,
                     std::string_view _applicationDescription,
                     float _applicationVersion,
                     std::string_view _contactAddress ) -> bool;

void error( const state_t& _state, const std::string& _message );

template < typename... Arguments >
void error( const state_t& _state,
            std::format_string< Arguments... > _format,
            Arguments&&... _arguments ) {
    const std::string l_message =
        std::format( _format, std::forward< Arguments >( _arguments )... );

    error( _state, l_message );
}

} // namespace arhodigp
