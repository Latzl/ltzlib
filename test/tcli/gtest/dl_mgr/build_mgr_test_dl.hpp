#pragma once
#include "common.hpp"
#include "ltz/dl_mgr.hpp"

#define LTZ_DL_MGR_NAME test_dl
#define LTZ_DL_MGR_SEQ ((int (*)(int, int), add))((void (*)(const char*), print))((size_t(*)(const char*), strlen))
#include LTZ_DL_MGR_BUILD_MGR()


#define LTZ_DL_MGR_NAMESPACE_SEQ (ns)(ns1)(ns2)(ns3)
#define LTZ_DL_MGR_NAME test_dl
#define LTZ_DL_MGR_SEQ ((int (*)(int, int), add))((void (*)(const char*), print))((size_t(*)(const char*), strlen))
#include LTZ_DL_MGR_BUILD_MGR()