#pragma once
#include "../_common/common.hpp"

#define FN_SCHEDULER(...) TCLI_FN(scheduler, __VA_ARGS__)
#define DF_SCHEDULER(desc, ...) TCLI_DF(desc, scheduler, __VA_ARGS__)
#define GF_SCHEDULER(...) TCLI_GTEST_FN(scheduler, __VA_ARGS__)
