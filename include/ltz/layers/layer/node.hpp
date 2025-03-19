#pragma once

#include "upper/sets.hpp"
#include "lower/sets.hpp"

namespace ltz {
namespace layers {

template <typename Derived, typename UpperSets, typename LowerSets>
class node : public std::enable_shared_from_this<Derived> {
   public:
    using upper_sets = UpperSets;
    using lower_sets = LowerSets;

    struct _layer {
        upper_sets upper;
        lower_sets lower;
    } layer;

    node() = default;

    /**
        @brief Construct a new node object
        
        @tparam T 
        @param wp 
        @note derived class must define a delegating constructor of this constructor
     */
    template <typename T>
    node(std::weak_ptr<T> wp) {
        static_assert(UpperSets::template contain<T>::value, "T is not one of types in UpperSets");
        layer.upper.add(wp);
    }

    /**
        @brief make shared ptr of Derived type
        
        @tparam Ts 
        @param args 
        @return std::shared_ptr<Derived> 
     */
    template <typename... Ts>
    static std::shared_ptr<Derived> make_shared(Ts&&... args) {
        return std::make_shared<Derived>(std::forward<Ts>(args)...);
    }

    /**
        @brief make lower node
        
        @tparam T 
        @tparam Ts 
        @param args 
        @return std::shared_ptr<T> 
     */
    template <typename T, typename... Ts>
    std::shared_ptr<T> make_lower(Ts&&... args) {
        static_assert(LowerSets::template contain<T>::value, "T is not one of types in LowerSets");
        auto sp = std::make_shared<T>(this->shared_from_this(), std::forward<Ts>(args)...);
        layer.lower.add(sp);
        return sp;
    }
};
}  // namespace layers
}  // namespace ltz