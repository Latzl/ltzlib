#pragma once
#include "loop_worker.hpp"

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
    std::lock_guard<std::mutex> lck{mtx_};
    if (running_) {
        return;
    }
    running_ = true;
    boost::asio::post(th_poll_, [this]() { do_work(boost::system::error_code{}); });
}

inline void loop_worker::stop() {
    std::lock_guard<std::mutex> lck{mtx_};
    running_ = false;
    work_timer_.cancel();
    std::promise<void> prom;
    std::future<void> fut = prom.get_future();
    boost::asio::post(th_poll_, [&prom]() { prom.set_value(); });
    fut.get();
}

inline void loop_worker::notify() {
    std::lock_guard<std::mutex> lck{mtx_};
    if (!running_) {
        return;
    }
    work_timer_.cancel();
    boost::asio::post(th_poll_, [this]() { do_work(boost::system::error_code{}); });
}

inline void loop_worker::notify_wait() {
    std::lock_guard<std::mutex> lck{mtx_};
    if (!running_) {
        return;
    }
    work_timer_.cancel();

    work_timer_.expires_after(conf::interval_time_t(conf_.interval_time));
    work_timer_.async_wait([this](const boost::system::error_code &ec) { do_work(ec); });

    work_timer_.wait();
}

inline void loop_worker::do_work(const boost::system::error_code &ec) {
    try {
        if (ec) {
            if (ec == boost::asio::error::operation_aborted) {
                return;
            }
            return;
        }
        fn_work_();
        if (running_) {
            std::lock_guard<std::mutex> lck{mtx_};
            work_timer_.expires_after(conf::interval_time_t(conf_.interval_time));
            work_timer_.async_wait([this](const boost::system::error_code &ec) { do_work(ec); });
        }
    } catch (...) {
    }
}
}  // namespace scheduler
}  // namespace ltz