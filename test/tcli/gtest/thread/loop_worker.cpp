#include "common.hpp"
#include "ltz/thread/loop_worker.hpp"

#define GF(...) GF_THREAD(loop_worker, __VA_ARGS__)

class TestThread {
   public:
    TestThread() {}
    ~TestThread() {
        th.stop();
    }

   private:
    int i{0};
    void work() {
        // std::cout << i++ << std::endl;
        i++;
    }

   public:
    ltz::thread::loop_worker th;
    void initTh() {
        th = ltz::thread::loop_worker("testTh", [this]() { work(); });
    }
};

GF(main_use, 0) {
    TestThread testTh;
    EXPECT_TRUE(!testTh.th);
    testTh.initTh();
    EXPECT_TRUE(testTh.th);
    EXPECT_FALSE(testTh.th.is_started());

    testTh.th.start();
    EXPECT_TRUE(testTh.th.is_started());
    EXPECT_TRUE(testTh.th.thWork_.joinable());
    testTh.th.stop();
    EXPECT_FALSE(testTh.th.is_started());
    EXPECT_FALSE(testTh.th.thWork_.joinable());
}

GF(config, eq) {
    using namespace ltz::thread;
    loop_worker_cfg cfg;
    cfg.sleep_time_ms = std::chrono::milliseconds(100 * 1000);
    loop_worker th, th1{"th1", nullptr, cfg};
    EXPECT_EQ(th1.cfg_.sleep_time_ms, std::chrono::milliseconds(100 * 1000));
}

GF(notify_work) {
    // using namespace utils::thread;
    int cnt = 0;
    ltz::thread::loop_worker_cfg cfg;
    cfg.sleep_time_ms = std::chrono::hours(999);
    ltz::thread::loop_worker th{"th", [&cnt]() { cnt++; }, cfg};
    th.start();
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    EXPECT_EQ(cnt, 1);
    th.notify_work();
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    EXPECT_EQ(cnt, 2);
    th.notify_work();
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    EXPECT_EQ(cnt, 3);
    th.stop();
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    EXPECT_EQ(cnt, 3);
}
GF(notify_stop) {
    // using namespace utils::thread;
    ltz::thread::loop_worker_cfg cfg;
    ltz::thread::loop_worker th{"th", []() { std::this_thread::sleep_for(std::chrono::milliseconds(5)); }, cfg};
    th.start();
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    th.notify_stop();
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    EXPECT_FALSE(th.is_started());

    th.start();
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    th.notify_stop();
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    EXPECT_FALSE(th.is_started());
}

GF(high_concurrency, 0) {
    TestThread testTh;
    testTh.initTh();
    ASSERT_TRUE(testTh.th);
    auto& internalThread = testTh.th;
    auto fstart = std::function<void()>([&internalThread]() { internalThread.start(); });
    auto fstop = std::function<void()>([&internalThread]() { internalThread.stop(); });
    std::vector<std::thread> threads;
    for (int i = 0; i < 10000; i++) {
        threads.emplace_back((i % 2 == 0 ? fstart : fstop));
    }
    // std::cout << "start/stoped" << std::endl;
    for (auto& th : threads) {
        if (th.joinable()) {
            th.join();
        }
    }
    // std::cout << "all joined" << std::endl;
    // std::cout << "stop" << std::endl;
    internalThread.stop();
    // std::cout << "stoped" << std::endl;
    EXPECT_FALSE(internalThread.bStarted_);
    EXPECT_FALSE(internalThread.thWork_.joinable());
}

GF(high_concurrency, long_sleep_time) {
    TestThread testTh;
    EXPECT_TRUE(!testTh.th);
    ltz::thread::loop_worker_cfg cfg;
    cfg.sleep_time_ms = std::chrono::milliseconds(10 * 1000);
    testTh.th = ltz::thread::loop_worker("testTh", [&testTh]() { testTh.work(); }, cfg);
    ASSERT_TRUE(testTh.th);
    ASSERT_EQ(testTh.th.cfg_.sleep_time_ms, std::chrono::milliseconds(10 * 1000));
    auto& internalThread = testTh.th;
    auto fstart = std::function<void()>([&internalThread]() { internalThread.start(); });
    auto fstop = std::function<void()>([&internalThread]() { internalThread.stop(); });
    std::vector<std::thread> threads;
    for (int i = 0; i < 10000; i++) {
        threads.emplace_back((i % 2 == 0 ? fstart : fstop));
    }
    // std::cout << "start/stoped" << std::endl;
    for (auto& th : threads) {
        if (th.joinable()) {
            th.join();
        }
    }
    // std::cout << "all joined" << std::endl;
    // std::cout << "stop" << std::endl;
    internalThread.stop();
    // std::cout << "stoped" << std::endl;
    EXPECT_FALSE(internalThread.bStarted_);
    EXPECT_FALSE(internalThread.thWork_.joinable());
}
