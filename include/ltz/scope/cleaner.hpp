#pragma once
#include <functional>

namespace ltz {

struct scope_cleaner {
    std::function<void()> fnClean;
    scope_cleaner(std::function<void()> fn = nullptr) : fnClean(fn) {}
    ~scope_cleaner() {
        if (fnClean) {
            fnClean();
        }
    }

    void cancel() {
        fnClean = nullptr;
    }
};
}  // namespace ltz