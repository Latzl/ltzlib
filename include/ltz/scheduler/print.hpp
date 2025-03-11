#pragma once
#include <fmt/core.h>
#include <fmt/chrono.h>
#include <boost/system.hpp>

namespace ltz {

inline int &print_level(){
    static int level{4};
    return level;
}

inline std::string humanize(std::chrono::system_clock::time_point tp) {
    auto now_time_t = std::chrono::system_clock::to_time_t(tp);
    std::tm local_tm = *std::localtime(&now_time_t);

    auto since_epoch = tp.time_since_epoch();
    auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(since_epoch) % 1000;

    return fmt::format("{:%F %T}.{}", local_tm, milliseconds.count());
}

template <typename... Ts>
void print(int level, Ts &&...args) {
    if (level <= print_level()) {
        boost::system::error_code ec(errno, boost::system::system_category());
        FILE *fp = level <= 3 ? stderr : stdout;
        // clang-format off
            fmt::print(fp, "[{}][{}][{}:{}]: {}\n",
                level,
                humanize(std::chrono::system_clock::now()),
                ec.value(),
                ec.message(),
                fmt::format(std::forward<Ts>(args)...)
            );
        // clang-format on
    }
}

template <typename... Ts>
void print_debug(Ts &&...args) {
    print(7, std::forward<Ts>(args)...);
}

template <typename... Ts>
void print_error(Ts &&...args) {
    print(3, std::forward<Ts>(args)...);
}

}  // namespace ltz