#include <print>

#include "stdfunc.hpp"

namespace {

[[gnu::constructor]] void load() {
    std::println( ">>> {}Loaded{}", stdfunc::color::g_green,
                  stdfunc::color::g_reset );
}

[[gnu::destructor]] void unload() {
    std::println( "<<< {}Unloaded{}", stdfunc::color::g_red,
                  stdfunc::color::g_reset );
}

} // namespace
