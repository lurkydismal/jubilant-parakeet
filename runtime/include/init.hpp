#pragma once

#include "runtime.hpp"
#include "stdfunc.hpp"

namespace runtime {

EXPORT auto init( applicationState_t& _applicationState,
                  int _argumentCount,
                  char** _argumentVector ) -> bool;

}
