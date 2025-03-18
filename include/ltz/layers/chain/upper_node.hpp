#pragma once
#include <memory>

namespace ltz {
namespace layers {
namespace chain {
namespace upper {
template <typename T>
class node {
   private:
    using type = T;

   public:
    std::weak_ptr<type> upper;
};
}  // namespace upper
}  // namespace chain
}  // namespace layers
}  // namespace ltz