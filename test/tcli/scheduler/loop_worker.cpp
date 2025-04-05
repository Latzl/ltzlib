#include "common.hpp"
#include <ltz/scheduler/loop_worker.hpp>
#include <spdlog/spdlog.h>
#include <fmt/chrono.h>

#define FN(...) FN_SCHEDULER(loop_worker, __VA_ARGS__)
#define DF(desc, ...) DF_SCHEDULER(desc, loop_worker, __VA_ARGS__)

// ltz::scheduler::loop_worker::conf g_cfg;
// struct Initer {
//     Initer() {  //
//         // g_cfg.debug_level = 7;
//     }
// } g_initer;

using namespace ltz::scheduler;
FN(0) {
    auto print_state = [](const auto& worker) {
        worker.sm_.visit_current_states([](const auto& state) { fmt::print("State: {}\n", state.c_str()); });
    };
    {
        loop_worker worker;
        worker.start();
        print_state(worker);
        worker.sm_.process_event(loop_worker::event::do_work{});
        print_state(worker);
        worker.sm_.process_event(loop_worker::event::stop{});
        print_state(worker);
        worker.sm_.process_event(loop_worker::event::work_done{});
        print_state(worker);
        worker.sm_.process_event(loop_worker::event::start{});
        print_state(worker);
        worker.sm_.process_event(loop_worker::event::do_work{});
        print_state(worker);
        worker.sm_.process_event(loop_worker::event::work_done{});
        print_state(worker);
        worker.stop();
        print_state(worker);
    }

    return 0;
}

FN(1) {
    loop_worker::conf_t cfg;
    cfg.interval_time = std::chrono::milliseconds(10);
    int cnt{0};
    loop_worker worker{
        [&cnt] {
            cnt++;
            fmt::print("{}\n", std::chrono::system_clock::now());
        },
        cfg  //
    };

    worker.start();

    std::this_thread::sleep_for(std::chrono::seconds(7));
    spdlog::info("cnt: {}", cnt);

    return 0;
}

FN(2) {
    int cnt = 0;
    loop_worker::conf_t cfg;
    cfg.interval_time = std::chrono::seconds(1);
    auto fn = [&cnt] {
        spdlog::info("done");
        cnt++; 
    }; 
    loop_worker worker{fn};

    worker.start();
    worker.start();
    std::this_thread::sleep_for(std::chrono::milliseconds(1));  // ensure the work begun
    spdlog::info("to stop");
    worker.stop();
    spdlog::info("stop done");
    spdlog::info("cnt: {}", cnt);

    SPDLOG_DEBUG("1111");

    return 0;
}