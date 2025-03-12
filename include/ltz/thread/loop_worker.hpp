#pragma once

#include <iostream>
#include "../scope/cleaner.hpp"
#include <atomic>
#include <condition_variable>
#include <functional>
#include <mutex>
#include <thread>

namespace ltz {

namespace thread {
struct loop_worker_cfg {
    std::chrono::milliseconds sleep_time_ms{1000};
};

class loop_worker {
   public:
    loop_worker(const std::string &_name = "", std::function<void()> _fn = nullptr, loop_worker_cfg _cfg = loop_worker_cfg{}) : name_(_name), fnWork_(_fn), cfg_(_cfg) {}

    ~loop_worker() {
        stop();
    }

    inline void start() {
        std::lock_guard<std::mutex> lckThis(mtxThis_);
        if (bStarted_) {
            return;
        }
        {
            if (thWork_.joinable()) {
                thWork_.join();
            }
            std::lock_guard<std::mutex> lckTh(mtxWorkThread_);
            bStarted_ = true;
            thWork_ = std::thread(&loop_worker::work, this);
        }
    }

    inline void stop() {
        std::lock_guard<std::mutex> lckThis(mtxThis_);
        scope::cleaner clr([this] {
            if (thWork_.joinable()) {
                thWork_.join();
            }
            bReadyStop_.store(false);
        });
        if (!bStarted_) {
            return;
        }
        {
            std::unique_lock<std::mutex> lckTh(mtxWorkThread_);
            cvReadyStop_.wait(lckTh, [this] { return bReadyStop_.load(); });
            bStarted_ = false;
            cvWork_.notify_one();
        }
    }

    inline bool is_started() const {
        std::lock_guard<std::mutex> lck(mtxThis_);
        return bStarted_;
    }

    inline void notify_work() {
        std::lock_guard<std::mutex> lckThis(mtxThis_);
        if (!bStarted_) {
            return;
        }
        cvWork_.notify_one();
    }

    inline void notify_stop() {
        std::lock_guard<std::mutex> lckThis(mtxThis_);
        bNeedStop_.store(true);
        cvWork_.notify_one();
    }

    inline loop_worker &operator=(const loop_worker &rhs) {
        std::lock_guard<std::mutex> lck(mtxThis_);
        name_ = rhs.name_;
        fnInit_ = rhs.fnInit_;
        fnWork_ = rhs.fnWork_;
        fnClear_ = rhs.fnClear_;
        cfg_ = rhs.cfg_;
        return *this;
    }

    inline operator bool() const {
        std::lock_guard<std::mutex> lck(mtxThis_);
        return !name_.empty() && fnWork_;
    }

   private:
    std::string name_;  // global unique name
    mutable std::mutex mtxThis_;

    std::atomic_bool bStarted_{false};
    std::atomic_bool bNeedStop_{false};
    std::thread thWork_;
    std::condition_variable cvWork_;
    mutable std::mutex mtxWorkThread_;

    std::function<void()> fnInit_{nullptr};
    std::function<void()> fnWork_{nullptr};
    std::function<void()> fnClear_{nullptr};

   public:
    inline bool set_init_fn(const std::function<void()> &fn) {
        std::lock_guard<std::mutex> lckThis(mtxThis_);
        if (bStarted_) {
            /* Thread is started, not set*/
            return false;
        }
        fnInit_ = fn;
        return true;
    }
    inline bool set_clear_fn(const std::function<void()> &fn) {
        std::lock_guard<std::mutex> lckThis(mtxThis_);
        if (bStarted_) {
            /* Thread is started, not set*/
            return false;
        }
        fnClear_ = fn;
        return true;
    }

   private:
    std::atomic_bool bReadyStop_{false};
    std::condition_variable cvReadyStop_;

    loop_worker_cfg cfg_;

    inline void work() noexcept{
        std::unique_lock<std::mutex> lock(mtxWorkThread_);
        bReadyStop_.store(true);
        cvReadyStop_.notify_one();
        bNeedStop_.store(false);
        scope::cleaner clr([this]() {
            bStarted_.store(false);
            bReadyStop_.store(false);
        });
        if (fnInit_) {
            try {
                fnInit_();
            } catch (const std::exception &e) {
                std::cerr << "[" << name_ << "] Init function exception: " << e.what() << std::endl;
                return;
            }
        }
        do {
            try {
                fnWork_();
            } catch (const std::exception &e) {
                std::cerr << "[" << name_ << "] Work function exception: " << e.what() << std::endl;
            }
            if (bNeedStop_ || !bStarted_) {
                break;
            }
            cvWork_.wait_for(lock, cfg_.sleep_time_ms);
            if (bNeedStop_ || !bStarted_) {
                break;
            }
        } while (1);
        if (fnClear_) {
            try {
                fnClear_();
            } catch (const std::exception &e) {
                std::cerr << "[" << name_ << "] Clear function exception: " << e.what() << std::endl;
            }
        }
    }
};
}  // namespace thread
}  // namespace ltz