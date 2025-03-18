#pragma once
#include "upper/sets.hpp"
#include "lower/sets.hpp"

namespace ltz {
namespace layers {

template <typename UpperSets, typename LowerSets>
class layer {
   public:
    UpperSets upper;
    LowerSets lower;
};

}  // namespace layers
}  // namespace ltz