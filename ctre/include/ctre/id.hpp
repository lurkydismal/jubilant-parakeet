#pragma once

#include "ctll/fixed_string.hpp"

#ifndef CTRE_IN_A_MODULE
#include <type_traits>
#endif

namespace ctre {

template < auto... Name >
struct id {
    static constexpr auto name =
        ctll::fixed_string< sizeof...( Name ) >{ { Name... } };

    friend constexpr auto operator==( id< Name... >, id< Name... > ) 
        -> std::true_type {
        return {};
    }

    template < auto... Other >
    friend constexpr auto operator==( id< Name... >, id< Other... > ) 
        -> std::false_type {
        return {};
    }

    template < typename T >
    friend constexpr auto operator==( id< Name... >, T ) 
        -> std::false_type {
        return {};
    }

    template < typename T >
    friend constexpr auto operator==( T, id< Name... > ) 
        -> std::false_type {
        return {};
    }
};

} // namespace ctre
