#include "common.hpp"
#include "ltzlib/dl_mgr.hpp"
#include "build_mgr_test_dl.hpp"

#define GF(...) GF_DL_MGR(build_mgr, __VA_ARGS__)


GF(construct) {
    ASSERT_NO_THROW([] { test_dl obj(DL_TEST_PATH); }());
}

GF(ptr) {
    test_dl obj(DL_TEST_PATH);
    EXPECT_TRUE(obj.add);
    EXPECT_TRUE(obj.print);
    EXPECT_TRUE(obj.strlen);
}

GF(add) {
    test_dl mgr(DL_TEST_PATH);
    EXPECT_EQ(mgr.add(1, 1), 2);
    EXPECT_EQ(mgr.add(-1, 1), 0);
}

GF(strlen) {
    test_dl mgr(DL_TEST_PATH);
    EXPECT_EQ(mgr.strlen("hello"), 5);
}

GF(namespace) {
    ASSERT_NO_THROW([] { ns::ns1::ns2::ns3::test_dl obj(DL_TEST_PATH); }());
}
