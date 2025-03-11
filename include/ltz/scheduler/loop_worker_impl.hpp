#pragma once
#include "loop_worker.hpp"
#include "print.hpp"

namespace ltz {
namespace scheduler {

inline loop_worker::loop_worker(std::function<void()> _fn_work, conf _conf = conf{})
    : fn_work_(_fn_work), conf_(_conf) {}

inline loop_worker::~loop_worker() {
    stop();
    th_poll_.stop();
    th_poll_.join();
}

inline void loop_worker::start() {
    std::lock_guard<std::mutex> lck{this_mtx_};
    print_debug("start() enter");
    if (running_) {
        print_debug("already started, return");
        return;
    }
    running_ = true;

    work_after_ul(conf::interval_time_t{0});
    print_debug("start() done");
}

inline void loop_worker::stop() {
    {
        std::lock_guard<std::mutex> lck{this_mtx_};
        running_ = false;
        work_timer_.cancel();
    }
    std::promise<void> prom;
    std::future<void> fut = prom.get_future();
    boost::asio::post(th_poll_, [&prom]() { prom.set_value(); });
    fut.get();
}

inline bool loop_worker::is_running() const {
    return running_;
}

template <typename Rep, typename Period>
void loop_worker::work_after(std::chrono::duration<Rep, Period> tp) {
    std::lock_guard<std::mutex> lck{this_mtx_};
    work_after_ul(tp);
}

inline void loop_worker::notify() {
    std::lock_guard<std::mutex> lck{this_mtx_};
    print_debug("notify() enter");
    work_after_ul(conf::interval_time_t{0});
    print_debug("notify() done");
}

inline void loop_worker::notify_wait() {
    std::lock_guard<std::mutex> lck{this_mtx_};
    if (!running_) {
        return;
    }
    work_timer_.cancel();

    work_after_ul(conf::interval_time_t{0});

    work_timer_.wait();
}

inline void loop_worker::do_work(const boost::system::error_code &ec) {
    print_debug("do_work() enter");
    try {
        if (ec) {
            if (ec == boost::asio::error::operation_aborted) {
                print_debug("operation_aborted");
                return;
            }
            print_debug("error[{}]: {}", ec.value(), ec.message());
            return;
        }
        std::unique_lock<std::mutex> work_lck(work_mtx_, std::try_to_lock);
        if (!work_lck.owns_lock()) {
            std::lock_guard<std::mutex> this_lck{this_mtx_};
            print_debug("work_lck not locked, set handler");
            next_handler_ = static_work_handler_;  // set handler for next work immediately
            return;
        }
        {
            std::lock_guard<std::mutex> this_lck{this_mtx_};
            print_debug("cancel all next");
            work_timer_.cancel();
            next_handler_ = nullptr;
        }

        fn_work_();

        {
            std::lock_guard<std::mutex> this_lck{this_mtx_};
            if (running_) {
                if (next_handler_) {
                    print_debug("set next handler handle immediately");
                    work_timer_.expires_after(conf::interval_time_t{0});
                    work_timer_.async_wait(next_handler_);
                    next_handler_ = nullptr;
                } else {
                    print_debug("set next handler handle after interval");
                    work_after_ul(conf::interval_time_t{conf_.interval_time});
                }
            }
        }
    } catch (...) {
    }
}

template <typename Rep, typename Period>
inline void loop_worker::work_after_ul(std::chrono::duration<Rep, Period> tp) {
    print_debug("work_after_ul() enter");
    if (!running_) {
        print_debug("not running, return");
        return;
    }

    work_timer_.cancel();
    work_timer_.expires_after(tp);

    work_timer_.async_wait(static_work_handler_);

    print_debug("work_after_ul() done");
}

}  // namespace scheduler
}  // namespace ltz