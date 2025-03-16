#pragma once
#include <memory>
#include <set>

namespace ltz {
namespace layers {
namespace _detail {
namespace lower {

template <typename T>
using set = std::set<std::shared_ptr<T>, std::owner_less<std::shared_ptr<T>>>;

}  // namespace lower
}  // namespace _detail
}  // namespace layers
}  // namespace ltz