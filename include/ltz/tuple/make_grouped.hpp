#pragma once

#include <tuple>

namespace ltz {
namespace tuple {

/* 
    @brief Make a tuple of tuples of size GroupSize from the given arguments.
    @param[in] args  The arguments to group.
    @return A tuple of tuples of size GroupSize.
 */
template <std::size_t GroupSize, typename... Ts>
constexpr auto make_grouped(Ts &&...args);
}
}  // namespace ltz

#include "detail/make_grouped.hpp"