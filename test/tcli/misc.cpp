#include "_common/common.hpp"
#include <fmt/core.h>

#define FN(...) TCLI_FN(misc, __VA_ARGS__)
#define DF(desc, ...) TCLI_DF(desc, misc, __VA_ARGS__)
#define GF(...) TCLI_GTEST_FN(misc, __VA_ARGS__)

template <typename Dur, typename DurTol>
bool is_tolarence(Dur got, Dur target, DurTol tolerance_val) {
    auto diff = got - target;
    fmt::print("diff: {}\n", diff.count());
    auto abs_diff = diff.count() > 0 ? diff : -diff;
    fmt::print("abs_diff: {}\n", abs_diff.count());
    using CommonType = typename std::common_type<decltype(abs_diff), DurTol>::type;
    return CommonType(abs_diff) <= CommonType(tolerance_val);
}

template <typename Dur>
bool is_tolarence(Dur got, Dur target) {
    return is_tolarence(got, target, std::chrono::milliseconds(1));
}

FN(0) {
    fmt::print("{}\n", is_tolarence(std::chrono::seconds(1), std::chrono::seconds(1)));
    fmt::print("{}\n", is_tolarence(std::chrono::microseconds(1), std::chrono::microseconds(100)));
    fmt::print("{}\n", is_tolarence(std::chrono::microseconds(1), std::chrono::microseconds(1001)));
    fmt::print("{}\n", is_tolarence(std::chrono::microseconds(1), std::chrono::microseconds(1002)));

    return 0;
}