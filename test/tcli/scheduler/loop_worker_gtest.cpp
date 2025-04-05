#define LTZ_SCHEDULER_LOOP_WORKER_DEBUG

#include "common.hpp"
#include <ltz/scheduler/loop_worker.hpp>

#define GF(...) GF_SCHEDULER(loop_worker, __VA_ARGS__)
#define GF_SM(...) GF(state_machine, __VA_ARGS__)

using namespace ltz::scheduler;

namespace state_machine {

using state = _detail::loop_worker::state_machine_t::state;
using event = _detail::loop_worker::state_machine_t::event;

struct test_sm {
    using executor = _detail::loop_worker::executor;
    using state_machine_t = _detail::loop_worker::state_machine_t;
    using sm =
        sml::sm<state_machine_t, sml::thread_safe<std::mutex>, sml::dont_instantiate_statemachine_class, sml::testing>;

    executor ex_;
    state_machine_t state_machine_{ex_};
    sm sm_{state_machine_};

    sm& operator()() {
        return sm_;
    }
};

GF_SM(stopped, do_work_at_start) {
    test_sm sm;
    EXPECT_TRUE(sm().is(sml::state<state::stopped>));

    sm().set_current_states(sml::state<state::stopped>);
    sm().process_event(event::start{});
    EXPECT_TRUE(sm().is(sml::state<state::working>));
    sm().process_event(event::stop{});
    EXPECT_TRUE(sm().is(sml::state<state::working>));
    sm().process_event(event::work_done{});
    EXPECT_TRUE(sm().is(sml::state<state::stopped>));

    sm().set_current_states(sml::state<state::stopped>);
    sm().process_event(event::do_work{});
    EXPECT_TRUE(sm().is(sml::state<state::stopped>));
}

GF_SM(stopped, dont_work_at_start) {
    test_sm sm;
    sm.state_machine_.conf_.interval_time = 100;
    sm.state_machine_.conf_.do_work_at_start = false;
    EXPECT_TRUE(sm().is(sml::state<state::stopped>));

    sm().set_current_states(sml::state<state::stopped>);
    sm().process_event(event::start{});
    EXPECT_TRUE(sm().is(sml::state<state::idle>));
    sm().process_event(event::stop{});
    EXPECT_TRUE(sm().is(sml::state<state::stopped>));
    sm().process_event(event::start{});
    EXPECT_TRUE(sm().is(sml::state<state::idle>));

    sm().set_current_states(sml::state<state::stopped>);
    sm().process_event(event::do_work{});
    EXPECT_TRUE(sm().is(sml::state<state::stopped>));
}

GF_SM(idle) {
    test_sm sm;
    sm().set_current_states(sml::state<state::idle>);
    sm().process_event(event::do_work{});
    EXPECT_TRUE(sm().is(sml::state<state::working>));

    sm().set_current_states(sml::state<state::idle>);
    sm().process_event(event::stop{});
    EXPECT_TRUE(sm().is(sml::state<state::stopped>));
}

GF_SM(working) {
    test_sm sm;
    std::string current_state;

    // normal case
    sm().set_current_states(sml::state<state::working>);
    sm().process_event(event::work_done{});
    EXPECT_TRUE(sm().is(sml::state<state::idle>));

    // stop
    sm().set_current_states(sml::state<state::working>);
    sm().process_event(event::stop{});
    EXPECT_TRUE(sm().is(sml::state<state::working>));
    sm().process_event(event::work_done{});
    EXPECT_TRUE(sm().is(sml::state<state::stopped>));

    // work
    sm().set_current_states(sml::state<state::working>);
    sm().process_event(event::do_work{});
    sm().process_event(event::do_work{});  // ignore multiple do_work
    sm().process_event(event::do_work{});
    EXPECT_TRUE(sm().is(sml::state<state::working>));
    sm().process_event(event::work_done{});
    EXPECT_TRUE(sm().is(sml::state<state::working>));
    sm().process_event(event::work_done{});
    EXPECT_TRUE(sm().is(sml::state<state::idle>));

    sm().set_current_states(sml::state<state::working>);
    sm().process_event(event::do_work{});
    sm().process_event(event::stop{});
    sm().process_event(event::work_done{});
    EXPECT_TRUE(sm().is(sml::state<state::stopped>));

    sm().set_current_states(sml::state<state::working>);
    sm().process_event(event::stop{});
    sm().process_event(event::do_work{});
    sm().process_event(event::work_done{});
    EXPECT_TRUE(sm().is(sml::state<state::stopped>));
}
}  // namespace state_machine

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

loop_worker::conf_t g_cfg;
struct Initer {
    Initer() {
        g_cfg.interval_time = 2;
    }
} g_initer;

GF(base, 0) {
    int cnt{0};
    auto fn = [&cnt]() { cnt++; };
    loop_worker::conf_t cfg;
    cfg.interval_time = 10;
    loop_worker worker{fn, cfg};
    worker.start();  // post fn immediately
    EXPECT_FALSE(worker.is_stopped());
    worker.stop();
    EXPECT_TRUE(worker.is_stopped());
}

GF(base, 1) {
    int cnt{0};
    auto fn = [&cnt]() { cnt++; };
    loop_worker::conf_t cfg;
    cfg.interval_time = 10;
    loop_worker worker{fn, cfg};
    worker.start();  // post fn immediately
    std::this_thread::sleep_for(std::chrono::milliseconds(109));
    worker.stop();
    EXPECT_EQ(cnt, 11);
}

GF(start) {
    int cnt{0};
    auto fn = [&cnt]() { cnt++; };
    {
        loop_worker worker{fn};
        worker.start();
        worker.start();
        worker.start();
        worker.start();
        worker.start();
        EXPECT_FALSE(worker.is_stopped());
        worker.stop();
        EXPECT_TRUE(worker.is_stopped());
        EXPECT_EQ(cnt, 1);
    }
    {
        cnt = 0;
        loop_worker worker{fn};
        boost::asio::thread_pool pool;
        auto tp_start = std::chrono::steady_clock::now();
        for (int i = 0; i < 100; i++) {
            boost::asio::post(pool, [&worker]() { worker.start(); });
        }
        pool.join();
        EXPECT_FALSE(worker.is_stopped());
        worker.stop();
        auto tp_stop = std::chrono::steady_clock::now();
        auto dur_elapsed = tp_stop - tp_start;
        ASSERT_TRUE(dur_elapsed < std::chrono::seconds(1));
        EXPECT_TRUE(worker.is_stopped());
        EXPECT_EQ(cnt, 1);
    }
}

GF(notify, 0) {
    auto prom = std::make_shared<std::promise<void>>();
    std::future<void> fut = prom->get_future();
    auto fn = [prom]() { prom->set_value(); };

    loop_worker::conf_t cfg;
    cfg.interval_time = 1000;
    cfg.do_work_at_start = false;
    loop_worker worker{fn, cfg};
    worker.start();
    auto start_tp = std::chrono::steady_clock::now();
    worker.notify();
    fut.wait();
    auto end_tp = std::chrono::steady_clock::now();
    worker.stop();
    auto cost_time = end_tp - start_tp;
    EXPECT_LT(cost_time, std::chrono::milliseconds(cfg.interval_time));
}

GF(notify, 1) {
    int cnt{0};
    std::mutex mtx;
    std::condition_variable cv;
    bool ready = false;

    auto fn = [&cnt, &mtx, &cv, &ready]() {
        std::lock_guard<std::mutex> lk(mtx);
        cnt++;
        ready = true;
        cv.notify_one();
    };

    loop_worker::conf_t cfg;
    cfg.interval_time = 1000;
    loop_worker worker{fn, cfg};

    auto tp_start = std::chrono::steady_clock::now();
    {
        std::unique_lock<std::mutex> lk(mtx);
        ready = false;
        worker.start();
        cv.wait(lk, [&ready] { return ready; });
    }
    for (int i = 0; i < 4; ++i) {
        std::unique_lock<std::mutex> lk(mtx);
        ready = false;
        worker.notify();  // 触发任务执行
        cv.wait(lk, [&ready] { return ready; });
    }
    worker.stop();
    auto tp_stop = std::chrono::steady_clock::now();
    EXPECT_EQ(cnt, 5);

    auto cost_time = tp_stop - tp_start;
    EXPECT_LT(cost_time, std::chrono::milliseconds(cfg.interval_time));
}

GF(notify, long_time_work) {
    int cnt{0};
    std::mutex mtx;
    std::condition_variable cv;
    bool ready = false;

    auto fn = [&cnt, &mtx, &cv, &ready]() {
        // long time to work
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        std::lock_guard<std::mutex> lk(mtx);
        cnt++;
        ready = true;
        cv.notify_one();
    };
    loop_worker worker{fn};

    auto tp_start = std::chrono::steady_clock::now();
    worker.start();
    {
        std::unique_lock<std::mutex> lk(mtx);
        cv.wait(lk, [&ready] { return ready; });
    }
    worker.notify();
    {
        std::unique_lock<std::mutex> lk(mtx);
        cv.wait(lk, [&ready] { return ready; });
    }
    worker.stop();
    auto tp_stop = std::chrono::steady_clock::now();
    EXPECT_EQ(cnt, 2);

    auto cost_time = tp_stop - tp_start;
    EXPECT_LT(cost_time, std::chrono::milliseconds(5));
}