#pragma once

#if defined( __cplusplus )

#define EXPORT extern "C"
#define restrict __restrict
#define register

#else

#define EXPORT
#define constinit
#define constexpr
#define consteval

#endif
