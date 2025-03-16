#pragma once
#include "../detail/lower/set.hpp"
#include "../detail/basic_sets.hpp"

namespace ltz {
namespace layers {
namespace lower {

template <typename... Types>
class sets : public _detail::basic_sets<_detail::lower::set, Types...> {
    using basic_sets = _detail::basic_sets<_detail::lower::set, Types...>;

   public:
    template <typename T>
    bool add(std::shared_ptr<T> ptr) {
        return basic_sets::template add<T>(ptr);
    }

    /**
        @brief inplace add a shared_ptr to the lower set
        
        @tparam T shared_ptr<T>
        @tparam Ts 
        @param args 
        @return std::shared_ptr<T> 
     */
    template <typename T, typename... Ts>
    std::shared_ptr<T> make(Ts&&... args) {
        static_assert(basic_sets::template contain<T>::value, "type not match");
        auto sp = std::make_shared<T>(std::forward<Ts>(args)...);
        if (add(sp)) {
            return sp;
        } else {
            return nullptr;
        }
    }
};

}  // namespace lower
}  // namespace layers
}  // namespace ltz