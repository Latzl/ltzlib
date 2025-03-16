#pragma once
#include <memory>
#include <set>

namespace ltz {
namespace layers {
namespace _detail {

namespace upper {

template <typename T>
using set = std::set<std::weak_ptr<T>, std::owner_less<std::weak_ptr<T>>>;

}  // namespace upper
}  // namespace _detail
}  // namespace layers
}  // namespace ltz