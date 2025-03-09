#pragma once
/**
    @file loop_worker.hpp
    @brief a class calling a work function every specific interval time
    @version 0.1
    @date 2025-03-09
    
 */
#include <boost/asio.hpp>
#include <fmt/core.h>
#include <fmt/chrono.h>

namespace ltz {
namespace scheduler {

class loop_worker {
   public:
    struct conf {
        using interval_time_t = std::chrono::milliseconds;
        std::int64_t interval_time{1000};  // in ms
    };

   public:
    loop_worker(std::function<void()> _fn_work, conf _conf);
    ~loop_worker();

    /**
        @brief post fn_work_ immdiatly and start timer to call fn_work_ every conf_.interval_time ms.
        
     */
    void start();

    /**
        @brief cancel timer and wait until fn_work_ is done.
        
     */
    void stop();

    /**
        @brief notify and return immediately

     */
    void notify();

    /**
        @brief notify and wait until fn_work_ started

     */
    void notify_wait();


   private:
    std::function<void()> fn_work_{nullptr};

    conf conf_;

    std::mutex mtx_;

    bool running_{false};

    boost::asio::thread_pool th_poll_{1};
    boost::asio::steady_timer work_timer_{th_poll_};

    /* helper */
   private:
    void do_work(const boost::system::error_code &ec);
};
}  // namespace scheduler
}  // namespace ltz

#include "loop_worker_impl.hpp"