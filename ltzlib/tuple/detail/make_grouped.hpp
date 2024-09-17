#pragma once

#include <tuple>

namespace ltz {
namespace tuple {
namespace _detail {

template <std::size_t Offset, typename Tuple, std::size_t... InGrpIdx>
inline constexpr auto make_grouped_impl1(Tuple &&tpl, std::index_sequence<InGrpIdx...>) {
    return std::make_tuple(std::get<Offset + InGrpIdx>(tpl)...);
}

template <std::size_t GroupSize, typename Tuple, std::size_t... GrpIdxs>
inline constexpr auto make_grouped_impl(Tuple &&tpl, std::index_sequence<GrpIdxs...>) {
    return std::make_tuple(make_grouped_impl1<GrpIdxs * GroupSize>(tpl, std::make_index_sequence<GroupSize>{})...);
}

}  // namespace _detail

template <std::size_t GroupSize, typename... Ts>
inline constexpr auto make_grouped(Ts &&...args) {
    constexpr std::size_t total_size = sizeof...(Ts);
    static_assert(total_size % GroupSize == 0, "Total size must be divisible by group size");
    constexpr std::size_t group_count = total_size / GroupSize;

    auto grouped = _detail::make_grouped_impl<GroupSize>(std::make_tuple(args...), std::make_index_sequence<group_count>{});

    return grouped;
}

}  // namespace tuple
}  // namespace ltz