#pragma once

#include <tuple>

namespace ltz {
namespace tuple {
namespace _detail {
template <typename Tuple, typename Fn, std::size_t... Is>
inline void for_each_tuple_impl(Tuple &&tpl, Fn &&fn, std::index_sequence<Is...>) {
    int dummy[] = {(fn(std::get<Is>(std::forward<Tuple>(tpl))), 0)...};
    (void)dummy;  // dewarnning
}
}  // namespace _detail

template <typename Tuple, typename Fn>
inline void for_each_tuple(Tuple &&tpl, Fn &&fn) {
    constexpr auto tpl_size = std::tuple_size<typename std::decay<Tuple>::type>::value;
    _detail::for_each_tuple_impl(std::forward<Tuple>(tpl), std::forward<Fn>(fn), std::make_index_sequence<tpl_size>{});
}
}  // namespace tuple
}  // namespace ltz