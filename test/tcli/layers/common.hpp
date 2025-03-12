#pragma once
#include "../_common/common.hpp"

#define FN_LAYERS(...) TCLI_FN(scheduler, __VA_ARGS__)
#define DF_LAYERS(desc, ...) TCLI_DF(desc, scheduler, __VA_ARGS__)
#define GF_LAYERS(...) TCLI_GTEST_FN(scheduler, __VA_ARGS__)
