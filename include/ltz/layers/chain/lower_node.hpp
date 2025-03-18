#pragma once
#include <memory>

namespace ltz {
namespace layers {
namespace chain {
namespace lower {
template <typename T>
class node {
   private:
    using type = T;

   public:
    std::shared_ptr<type> upper;
};
}  // namespace lower
}  // namespace chain
}  // namespace layers
}  // namespace ltz