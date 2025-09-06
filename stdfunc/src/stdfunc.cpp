#include "stdfunc.hpp"

namespace stdfunc {

thread_local engine_t g_engine{ std::random_device{}() };

}
