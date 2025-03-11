#include "common.hpp"
#include <ltz/scheduler/loop_worker.hpp>
#include <fmt/core.h>
#include <fmt/chrono.h>

#define GF(...) GF_SCHEDULER(loop_worker, __VA_ARGS__)

namespace ltz_gtest {
ltz::scheduler::loop_worker::conf g_cfg;
struct Initer {
    Initer() {
        g_cfg.interval_time = 2;
        ltz::print_level() = 7;
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
    cfg.interval_time = 10;
    ltz::scheduler::loop_worker worker(fn, cfg);
    worker.start();  // post fn immediately
    EXPECT_TRUE(worker.is_running());
    worker.stop();
    EXPECT_FALSE(worker.is_running());
}

GF(start) {
    int cnt{0};
    auto fn = [&cnt]() { cnt++; };
    {
        ltz::scheduler::loop_worker worker(fn, g_cfg);
        worker.start();
        worker.start();
        worker.start();
        worker.start();
        worker.start();
        EXPECT_TRUE(worker.is_running());
        worker.stop();
        EXPECT_FALSE(worker.is_running());
    }
    {
        ltz::scheduler::loop_worker worker(fn, g_cfg);
        boost::asio::thread_pool pool;
        for (int i = 0; i < 100; i++) {
            boost::asio::post(pool, [&worker]() { worker.start(); });
        }
        pool.join();
        EXPECT_TRUE(worker.is_running());
        worker.stop();
        EXPECT_FALSE(worker.is_running());
    }
}

GF(notify) {
    std::promise<void> prom;
    std::future<void> fut = prom.get_future();
    auto fn = [&prom]() { prom.set_value(); };

    ltz::scheduler::loop_worker::conf cfg;
    cfg.interval_time = 1000;
    ltz::scheduler::loop_worker worker(fn, cfg);
    worker.start();
    auto start_tp = std::chrono::steady_clock::now();
    worker.notify();
    fut.get();
    auto end_tp = std::chrono::steady_clock::now();
    worker.stop();
    auto cost_time = std::chrono::duration_cast<std::chrono::milliseconds>(end_tp - start_tp);
    EXPECT_LT(cost_time, std::chrono::milliseconds(cfg.interval_time));
}

GF(notify_wait) {
    std::promise<void> prom;
    std::future<void> fut = prom.get_future();
    auto fn = [&prom]() { prom.set_value(); };

    ltz::scheduler::loop_worker::conf cfg;
    cfg.interval_time = 1000;
    ltz::scheduler::loop_worker worker(fn, cfg);
    worker.start();
    auto start_tp = std::chrono::steady_clock::now();
    worker.notify_wait();
    fut.get();
    auto end_tp = std::chrono::steady_clock::now();
    worker.stop();
    auto cost_time = std::chrono::duration_cast<std::chrono::milliseconds>(end_tp - start_tp);
    EXPECT_LT(cost_time, std::chrono::milliseconds(cfg.interval_time));
}
}  // namespace ltz_gtest