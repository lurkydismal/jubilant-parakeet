#pragma once

#include "ctll/list.hpp"
#include "pcre.hpp"
#include "utility.hpp"

namespace ctre {

struct singleline {};
struct multiline {};

struct case_sensitive {};
struct case_insensitive {};

using ci = case_insensitive;
using cs = case_sensitive;

template < typename... Flags >
struct flag_list {};

struct flags {
    bool block_empty_match = false;
    bool multiline = false;
    bool case_insensitive = false;

    constexpr flags() = default;
    constexpr flags( const flags& ) = default;
    constexpr flags( flags&& ) = default;

    constexpr CTRE_FORCE_INLINE flags( ctre::singleline v )  {
        set_flag( v );
    }
    constexpr CTRE_FORCE_INLINE flags( ctre::multiline v )  {
        set_flag( v );
    }
    constexpr CTRE_FORCE_INLINE flags( ctre::case_sensitive v )  {
        set_flag( v );
    }
    constexpr CTRE_FORCE_INLINE flags( ctre::case_insensitive v )  {
        set_flag( v );
    }

    template < typename... Args >
    constexpr CTRE_FORCE_INLINE flags( ctll::list< Args... > )  {
        ( this->set_flag( Args{} ), ... );
    }

    constexpr friend CTRE_FORCE_INLINE auto operator+(
        flags f,
        pcre::mode_case_insensitive )  {
        f.case_insensitive = true;
        return f;
    }

    constexpr friend CTRE_FORCE_INLINE auto operator+(
        flags f,
        pcre::mode_case_sensitive )  {
        f.case_insensitive = false;
        return f;
    }

    constexpr friend CTRE_FORCE_INLINE auto operator+(
        flags f,
        pcre::mode_singleline )  {
        f.multiline = false;
        return f;
    }

    constexpr friend CTRE_FORCE_INLINE auto operator+(
        flags f,
        pcre::mode_multiline )  {
        f.multiline = true;
        return f;
    }

    constexpr CTRE_FORCE_INLINE void set_flag( ctre::singleline )  {
        multiline = false;
    }

    constexpr CTRE_FORCE_INLINE void set_flag( ctre::multiline )  {
        multiline = true;
    }

    constexpr CTRE_FORCE_INLINE void set_flag(
        ctre::case_insensitive )  {
        case_insensitive = true;
    }

    constexpr CTRE_FORCE_INLINE void set_flag( ctre::case_sensitive )  {
        case_insensitive = false;
    }
};

constexpr CTRE_FORCE_INLINE auto not_empty_match( flags f ) {
    f.block_empty_match = true;
    return f;
}

constexpr CTRE_FORCE_INLINE auto consumed_something( flags f,
                                                     bool condition = true ) {
    if ( condition )
        f.block_empty_match = false;
    return f;
}

constexpr CTRE_FORCE_INLINE bool cannot_be_empty_match( flags f ) {
    return f.block_empty_match;
}

constexpr CTRE_FORCE_INLINE bool multiline_mode( flags f ) {
    return f.multiline;
}

constexpr CTRE_FORCE_INLINE bool is_case_insensitive( flags f ) {
    return f.case_insensitive;
}

} // namespace ctre
