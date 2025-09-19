#pragma once

#include <format>
#include <gsl/gsl>
#include <span>
#include <string_view>

namespace arhodigp {

using option_t = struct option {
    /* The long option name.  For more than one name for the
same option, you can use following options with the OPTION_ALIAS flag set.  */
    const char* name;

    /* What key is returned for this option.  If > 0 and printable, then it's
       also accepted as a short option.  */
    int key;

    /* If non-NULL, this is the name of the argument associated with this
       option, which is required unless the OPTION_ARG_OPTIONAL flag is set. */
    const char* arg;

    /* OPTION_ flags.  */
    int flags;

/* The argument associated with this option is optional.  */
#define OPTION_ARG_OPTIONAL 0x1

/* This option is an alias for the closest previous non-alias option.  This
   means that it will be displayed in the same help entry, and will inherit
   fields other than NAME and KEY from the aliased option.  */
#define OPTION_ALIAS 0x4

    /* The doc string for this option.  If both NAME and KEY are 0, This string
       will be printed outdented from the normal option column, making it
       useful as a group header (it will be the first thing printed in its
       group); in this usage, it's conventional to end the string with a `:'. */
    const char* doc;

    /* The group this option is in.  In a long help message, options are sorted
       alphabetically within each group, and the groups presented in the order
       0, 1, 2, ..., n, -m, ..., -2, -1.  Every entry in an options array with
       if this field 0 will inherit the group number of the previous entry, or
       zero if it's the first one, unless its a group header (NAME and KEY both
       0), in which case, the previous entry + 1 is the default.  Automagic
       options such as --help are put into group -1.  */
    int group;
};

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
