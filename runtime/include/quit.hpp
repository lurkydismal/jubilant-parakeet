#pragma once

#include "runtime.hpp"
#include "stdfunc.hpp"

namespace runtime {

EXPORT auto quit( applicationState_t& _applicationState, bool _result ) -> bool;

}
