#pragma once
#include <functional>

namespace ltz {

namespace scope {
struct cleaner {
    cleaner(std::function<void()> fn = nullptr) : clean_fn_(fn) {}
    ~cleaner() {
        if (clean_fn_) {
            clean_fn_();
        }
    }

    void cancel() {
        clean_fn_ = nullptr;
    }

   private:
    std::function<void()> clean_fn_;
};
}  // namespace scope
}  // namespace ltz