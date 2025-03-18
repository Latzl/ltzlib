#pragma once
#include "../detail/basic_sets.hpp"
#include <set>
#include <memory>

namespace ltz {
namespace layers {
namespace upper {

namespace _detail {
template <typename T>
using set = std::set<std::weak_ptr<T>, std::owner_less<std::weak_ptr<T>>>;
}

template <typename... Types>
class sets : public layers::_detail::basic_sets<_detail::set, Types...> {
    using basic_sets = layers::_detail::basic_sets<_detail::set, Types...>;

   public:
    template <typename T>
    void for_each(std::function<void(std::shared_ptr<T>)> func) {
        static_assert(basic_sets::template contain<T>::value, "type not match");
        auto& set = basic_sets::template get_set<T>();
        for (auto it = set.begin(); it != set.end();) {
            if (auto sp = it->lock()) {
                func(sp);
                it++;
            } else {
                it = set.erase(it);
            }
        }
    }

    template <typename T>
    bool add(std::weak_ptr<T> ptr) {
        return basic_sets::template add<T>(ptr);
    }

    template <typename T>
    bool add(std::shared_ptr<T> ptr) {
        return basic_sets::template add<T>(std::weak_ptr<T>(ptr));
    }

    template <typename T>
    void prune_expired() {
        static_assert(basic_sets::template contain<T>::value, "type not match");
        auto& set = basic_sets::template get_set<T>();
        for (auto it = set.begin(); it != set.end();) {
            if (it->expired()) {
                it = set.erase(it);
            } else {
                ++it;
            }
        }
    }

    void prune_expired() {
        std::initializer_list<int> unused{(prune_expired<Types>(), 0)...};
        (void)unused;
    }
};

}  // namespace upper
}  // namespace layers
}  // namespace ltz