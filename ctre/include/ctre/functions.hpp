#pragma once

#include "ctll.hpp"
#include "evaluation.hpp"
#include "id.hpp"
#include "pcre_actions.hpp"
#include "wrapper.hpp"

namespace ctre {

template < ctll::fixed_string input, typename Modifier = void >
CTRE_FLATTEN constexpr CTRE_FORCE_INLINE auto re() noexcept {
    constexpr auto _input = input; // workaround for GCC 9 bug 88092

    using tmp =
        typename ctll::parser< ctre::pcre, _input, ctre::pcre_actions >::
            template output< pcre_context<> >;
    static_assert( tmp(), "Regular Expression contains syntax error." );
    using regex = decltype( front( typename tmp::output_type::stack_type() ) );
    return ctre::regular_expression< regex, Modifier, singleline >();
}

} // namespace ctre
