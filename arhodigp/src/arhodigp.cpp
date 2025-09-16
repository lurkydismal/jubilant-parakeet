#include "arhodigp.hpp"

#include <argp.h>

#include <print>
#include <ranges>
#include <string>
#include <vector>

namespace arhodigp {

namespace {

std::string g_version;
const char* argp_program_version;

std::string g_contactAddress;
const char* argp_program_bug_address;

auto parserForOption( int _key, char* _value, argp_state* _state ) -> error_t {
    error_t l_returnValue = 0;

    switch ( _key ) {
        // Not flag/ option
        case ARGP_KEY_ARG: {
            if ( _value ) {
            }

            break;
        }

        // After all keys were processed
        case ARGP_KEY_END: {
            break;
        }

        // Unknown key
        default: {
            l_returnValue = ARGP_ERR_UNKNOWN;
        }
    }

    return ( l_returnValue );
}

} // namespace

// [NAME] - optional
// NAME - required
// NAME... - at least one and more
auto parseArguments( std::string& _format,
                     std::span< std::string_view > _arguments,
                     std::string_view _applicationIdentifier,
                     std::string_view _applicationDescription,
                     float _applicationVersion,
                     std::string_view _contactAddress ) -> bool {
    bool l_returnValue = false;

    do {
        g_version = _applicationVersion;
        argp_program_version = g_version.c_str();

        g_contactAddress = std::format( "<{}>", _contactAddress );
        argp_program_bug_address = g_contactAddress.c_str();

        {
            const std::vector< argp_option > l_options = {
                {
                    .name = "verbose",
                    .key = 'a',
                    .arg = nullptr,
                    .flags = 0,
                    .doc = "Produce verbose output",
                    .group = 0,
                },
                {
                    .name = nullptr,
                    .key = 0,
                    .arg = nullptr,
                    .flags = 0,
                    .doc = nullptr,
                    .group = 0,
                },
            };

            const std::string l_description = std::format(
                "{} - {}", _applicationIdentifier, _applicationDescription );

            argp l_argumentParser = {
                .options = l_options.data(),
                .parser = parserForOption,
                .args_doc = _format.c_str(),
                .doc = l_description.c_str(),
                .children = nullptr,
                .help_filter = nullptr,
                .argp_domain = nullptr,
            };

            // TODO: Improve
            const auto l_x = _arguments |
                             std::views::transform( []( auto _argument ) {
                                 return ( std::string( _argument ) );
                             } ) |
                             std::ranges::to< std::vector >();

            const auto l_y = l_x |
                             std::views::transform( []( auto& _argument ) {
                                 return ( _argument.c_str() );
                             } ) |
                             std::ranges::to< std::vector >();

            if ( argp_parse( &l_argumentParser, _arguments.size(),
                             std::bit_cast< char** >( l_y.data() ), 0, nullptr,
                             nullptr ) != 0 ) {
                break;
            }
        }

        l_returnValue = true;
    } while ( false );

    return ( l_returnValue );
}

auto parseArguments( std::string_view _format,
                     std::span< std::string_view > _arguments,
                     std::string_view _applicationIdentifier,
                     std::string_view _applicationDescription,
                     float _applicationVersion,
                     std::string_view _contactAddress ) -> bool {
    auto l_format = std::string( _format );

    return ( parseArguments( l_format, _arguments, _applicationIdentifier,
                             _applicationDescription, _applicationVersion,
                             _contactAddress ) );
}

void error( const state_t& _state, const std::string& _message ) {
    argp_error( std::bit_cast< const argp_state* >( _state ), "%s",
                _message.c_str() );
}

} // namespace arhodigp
