#include "common.hpp"
#include <ltz/scheduler/loop_worker.hpp>
#include <fmt/core.h>
#include <fmt/chrono.h>

#define FN(...) FN_SCHEDULER(loop_worker, __VA_ARGS__)
#define DF(desc, ...) DF_SCHEDULER(desc, loop_worker, __VA_ARGS__)

ltz::scheduler::loop_worker::conf g_cfg;
struct Initer {
    Initer() {  //
        // g_cfg.debug_level = 7;
    }
} g_initer;

FN(0) {
    try {
        ltz::scheduler::loop_worker worker{[] { fmt::print("hello world\n"); }, g_cfg};

        worker.start();
        std::this_thread::sleep_for(std::chrono::seconds(5));
        worker.stop();
        std::this_thread::sleep_for(std::chrono::seconds(1));
        worker.start();
        std::this_thread::sleep_for(std::chrono::seconds(5));
        worker.stop();
    } catch (const std::exception& e) {
        fmt::print(stderr, "exception: {}\n", e.what());
    } catch (...) {
        fmt::print(stderr, "unknown exception\n");
    }

    return 0;
}
FN(1) {
    try {
        ltz::scheduler::loop_worker worker{[] { fmt::print("hello world\n"); }, g_cfg};

        worker.start();
        worker.notify();
        worker.notify();
        worker.notify();
        worker.stop();
        std::this_thread::sleep_for(std::chrono::seconds(1));
    } catch (const std::exception& e) {
        fmt::print(stderr, "exception: {}\n", e.what());
    } catch (...) {
        fmt::print(stderr, "unknown exception\n");
    }

    return 0;
}
FN(2) {
    try {
        ltz::scheduler::loop_worker worker{
            [] {
                fmt::print("do enter\n");
                std::this_thread::sleep_for(std::chrono::seconds(1));
                fmt::print("do done\n");
            },
            g_cfg  //
        };

        worker.start();
        worker.notify();
        fmt::print("notify_await start\n");
        worker.notify_wait();
        fmt::print("notify_await done\n");
        worker.stop();
        std::this_thread::sleep_for(std::chrono::seconds(1));
    } catch (const std::exception& e) {
        fmt::print(stderr, "exception: {}\n", e.what());
    } catch (...) {
        fmt::print(stderr, "unknown exception\n");
    }

    return 0;
}