#pragma once
#include <boost/sml.hpp>
#include <boost/asio.hpp>
#include <functional>
#include <thread>
#include <mutex>

#ifdef LTZ_SCHEDULER_LOOP_WORKER_DEBUG
#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_DEBUG
#include <spdlog/spdlog.h>
#define DEBUG(...) SPDLOG_DEBUG(__VA_ARGS__)
#else
#define DEBUG(...)
#endif

namespace ltz {
namespace scheduler {

namespace sml = boost::sml;
namespace asio = boost::asio;

class loop_worker;

namespace _detail {
namespace loop_worker {

struct conf_t {
    conf_t() = default;
    using interval_time_t = std::chrono::steady_clock::duration;
    interval_time_t interval_time{std::chrono::seconds{1}};
    bool do_work_at_start{true};
};

struct executor {
    struct conf_t {
        std::function<void()> fn_work;
        std::function<void()> fn_work_done_callback;
    } conf;

    executor() = default;
    executor(conf_t c) : conf(c) {}

    void do_work() {
        if (conf.fn_work) {
            conf.fn_work();
        }
        if (conf.fn_work_done_callback) {
            conf.fn_work_done_callback();
        }
    };
};

struct state_machine_t {
    friend class loop_worker;

    explicit state_machine_t(executor& ex) : executor_(ex) {}
    ~state_machine_t() {
        DEBUG("~state_machine_t() enter");
        th_pool_.stop();
        th_pool_.join();
        DEBUG("~state_machine_t() exit");
    }

    struct event {
        struct start {};
        struct stop {};
        struct do_work {};
        struct work_done {};
    };

    struct state {
        struct stopped {};
        struct idle {};
        struct working {};
    };

    auto operator()() {
        using namespace sml;

        /* states */
        auto stopped = sml::state<state::stopped>;
        auto idle = sml::state<state::idle>;
        auto working = sml::state<state::working>;

        /* actions */
        // auto& sm, auto& deps, auto& subs: https://github.com/boost-ext/sml/issues/456
        auto set_work_timer = [this](auto, auto& sm, auto& deps, auto& subs) {
            work_timer_.expires_after(conf_t::interval_time_t{conf_.interval_time});
            work_timer_.async_wait([&](const boost::system::error_code& ec) mutable {
                DEBUG("time out handler enter, ec: {}:{}", ec.value(), ec.message());
                if (ec) {
                    if (ec == boost::asio::error::operation_aborted) {
                        return;
                    }
                    // TODO handle other error
                    return;
                }
                DEBUG("post do_work event");
                sm.process_event(event::do_work{}, deps, subs);
                DEBUG("time out handler exit");
            });
        };
        auto cancel_work_timer = [this] { work_timer_.cancel(); };
        auto post_work = [this]() { asio::post(th_pool_, [this]() { executor_.do_work(); }); };
        auto on_stop_entry = [this] { stop_after_work_done_.store(false); };
        auto on_work_entry = [this] { work_immediately_.store(false); };


        /* gaurd */
        auto is_need_do_work_at_start = [this] { return conf_.do_work_at_start; };
        auto is_need_stop_after_work_done = [this] { return stop_after_work_done_.load(); };
        auto is_need_work_immediately = [this] { return work_immediately_.load(); };

        // clang-format off
        return make_transition_table(
            *stopped + sml::event<event::start> [ is_need_do_work_at_start ] / post_work = working,
            stopped + sml::event<event::start> / set_work_timer = idle,
            stopped + sml::event<event::stop> = stopped,

            idle + sml::event<event::stop> / cancel_work_timer = stopped,
            idle + sml::event<event::do_work> / (cancel_work_timer, post_work) = working,

            working + sml::event<event::work_done> [ is_need_stop_after_work_done ] = stopped,
            working + sml::event<event::work_done> [ is_need_work_immediately ] / post_work = working,
            working + sml::event<event::work_done> / set_work_timer = idle,
            // non work_done event when working
            working + sml::event<event::stop> / [this] { stop_after_work_done_.store(true); },
            working + sml::event<event::do_work> / [this] { work_immediately_.store(true); },
            // clear work done flag
            stopped + sml::on_entry<_> / on_stop_entry,
            working + sml::on_entry<_> / on_work_entry
        );
        // clang-format on
    }

   private:
    conf_t conf_;

    executor& executor_;

    asio::thread_pool th_pool_{1};
    asio::steady_timer work_timer_{th_pool_};

    // work done gaurd flag
    std::atomic_bool stop_after_work_done_{false};
    std::atomic_bool work_immediately_{false};
};

}  // namespace loop_worker
}  // namespace _detail

class loop_worker {
   private:
    using executor = _detail::loop_worker::executor;
    using state_machine_t = _detail::loop_worker::state_machine_t;
    using event = state_machine_t::event;
    using state = state_machine_t::state;

   public:
    using conf_t = _detail::loop_worker::conf_t;

   private:
    loop_worker() {
        executor_.conf.fn_work_done_callback = [this] { sm_.process_event(event::work_done{}); };
    }

   public:
    loop_worker(conf_t conf) : loop_worker() {
        state_machine_.conf_ = conf;
    }
    loop_worker(std::function<void()> fn_work, conf_t conf = conf_t{}) : loop_worker(conf) {
        executor_.conf.fn_work = fn_work;
    }
    ~loop_worker() {
        DEBUG("~loop_worker() enter");
        stop();
        DEBUG("~loop_worker() exit");
    }

    /**
        @brief start
        
     */
    void start() {
        sm_.process_event(event::start{});
    }

    /**
        @brief stop
        
     */
    void stop() {
        DEBUG("stop() enter");
        sm_.process_event(event::stop{});

        // wait until work done
        auto& thpool = state_machine_.th_pool_;
        auto prom = std::make_shared<std::promise<void>>();

        auto fut = prom->get_future();
        asio::post(thpool, [prom] {
            DEBUG("in stop() post lambda enter");
            prom->set_value();
            DEBUG("in stop() post lambda exit");
        });
        fut.wait();
        DEBUG("stop() exit");
    }

    /**
        @brief 
        
     */
    bool is_stopped() const {
        return sm_.is(sml::state<state::stopped>);
    }

    /**
        @brief notify to do work imediately
        
     */
    void notify() {
        sm_.process_event(event::do_work{});
    }

   private:
    executor executor_;
    state_machine_t state_machine_{executor_};

    // clang-format off
    sml::sm<
        state_machine_t,
        sml::thread_safe<std::mutex>, sml::dont_instantiate_statemachine_class
    > sm_{state_machine_};
    // clang-format on
};

}  // namespace scheduler2
}  // namespace ltz