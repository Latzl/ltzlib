#include "common.hpp"
#include <ltz/scheduler/loop_worker.hpp>

#define GF(...) GF_SCHEDULER(loop_worker, __VA_ARGS__)

namespace ltz_gtest {
ltz::scheduler::loop_worker::conf g_cfg;
struct Initer {
    Initer() {
        g_cfg.interval_time = 2;
    }
} g_initer;

template <typename Dur, typename DurTol>
bool is_tolarence(Dur got, Dur target, DurTol tolerance_val) {
    auto diff = got - target;
    auto abs_diff = diff.count() > 0 ? diff : -diff;
    using CommonType = typename std::common_type<decltype(abs_diff), DurTol>::type;
    return CommonType(abs_diff) <= CommonType(tolerance_val);
}

template <typename Dur>
bool is_tolarence(Dur got, Dur target) {
    return is_tolarence(got, target, std::chrono::microseconds(100));
}

GF(is_tolarence) {
    EXPECT_TRUE(is_tolarence(std::chrono::milliseconds(1), std::chrono::milliseconds(1)));
    EXPECT_TRUE(is_tolarence(std::chrono::microseconds(2), std::chrono::microseconds(1)));
    EXPECT_TRUE(is_tolarence(std::chrono::microseconds(101), std::chrono::microseconds(1)));
    EXPECT_FALSE(is_tolarence(std::chrono::microseconds(102), std::chrono::microseconds(1)));
}


GF(base) {
    int cnt{0};
    auto fn = [&cnt]() { cnt++; };
    ltz::scheduler::loop_worker::conf cfg;
    cfg.interval_time = 1;
    ltz::scheduler::loop_worker worker(fn, cfg);
    worker.start();  // post fn immediately
    std::this_thread::sleep_for(std::chrono::microseconds(5500));
    worker.stop();
    EXPECT_EQ(cnt, 6);
}

GF(start) {
    int cnt{0};
    auto fn = [&cnt]() { cnt++; };
    ltz::scheduler::loop_worker::conf cfg;
    cfg.interval_time = 2;
    ltz::scheduler::loop_worker worker(fn, cfg);
    worker.start();
    worker.start();
    worker.start();
    worker.start();
    worker.start();
    std::this_thread::sleep_for(std::chrono::microseconds(1500));
    worker.stop();
    EXPECT_EQ(cnt, 1);
}

GF(notify) {
    int cnt{0};
    auto fn = [&cnt]() { cnt++; };
    ltz::scheduler::loop_worker::conf cfg;
    cfg.interval_time = 1000;
    ltz::scheduler::loop_worker worker(fn, cfg);
    worker.start();  // post fn immediately
    worker.notify();
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
    worker.notify();
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
    worker.notify();
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
    worker.stop();
    EXPECT_EQ(cnt, 4);
}

GF(notify_wait) {
    int cnt{0};
    auto fn = [&cnt]() {
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
        cnt++;
    };
    ltz::scheduler::loop_worker::conf cfg;
    cfg.interval_time = 5;
    ltz::scheduler::loop_worker worker(fn, cfg);
    auto start_tp = std::chrono::steady_clock::now();
    worker.start();  // post fn immediately
    worker.notify_wait();  // wait for fn to finish while at least 5ms is required to finish
    auto end_tp = std::chrono::steady_clock::now();
    auto cost_time = end_tp - start_tp;
    std::this_thread::sleep_for(std::chrono::microseconds(5500));  // wait for fn to finish
    worker.stop();
    EXPECT_EQ(cnt, 2);
    EXPECT_TRUE(
        is_tolarence(std::chrono::duration_cast<std::chrono::milliseconds>(cost_time), std::chrono::milliseconds(5)));
}
}  // namespace ltz_gtest