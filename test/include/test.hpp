#pragma once

#if defined( TESTS )

#include <gmock/gmock.h> // ElementsAre, HasSubstr, MatchesRegex ...
#include <gtest/gtest.h>

using namespace testing;

extern size_t g_seed;

#endif
