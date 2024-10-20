#pragma once

namespace ltz {
namespace tuple {

/* 
    @brief: Applies a function to each element of a tuple.
    @param tpl: The tuple to apply the function to.
    @param fn: The function to apply to each element of the tuple.
 */
template <typename Tuple, typename Fn>
void for_each_tuple(Tuple &&tpl, Fn &&fn);
}
}  // namespace ltz

#include "detail/for_each.hpp"