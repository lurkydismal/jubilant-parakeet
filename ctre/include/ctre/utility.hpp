#pragma once

#include "ctll/utilities.hpp"

#define CTRE_CNTTP_COMPILER_CHECK CTLL_CNTTP_COMPILER_CHECK

#ifdef CTRE_IN_A_MODULE
#define CTRE_EXPORT export
#else
#define CTRE_EXPORT
#endif

#if __GNUC__ > 9
#if __has_cpp_attribute( likely )
#define CTRE_LIKELY [[likely]]
#else
#define CTRE_LIKELY
#endif

#if __has_cpp_attribute( unlikely )
#define CTRE_UNLIKELY [[unlikely]]
#else
#define CTRE_UNLIKELY
#endif
#else
#define CTRE_LIKELY
#define CTRE_UNLIKELY
#endif

#define CTRE_FORCE_INLINE inline __attribute__( ( always_inline ) )
#define CTRE_FLATTEN __attribute__( ( flatten ) )
