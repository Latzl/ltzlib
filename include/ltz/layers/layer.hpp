#pragma once
#include <boost/fusion/include/map.hpp>
#include <boost/fusion/include/at_key.hpp>
#include <set>
#include <memory>
#include <type_traits>

namespace ltz {
namespace layers {

template <typename T, typename... Us>
struct is_in_list : std::false_type {};

template <typename T, typename First, typename... Rest>
struct is_in_list<T, First, Rest...>
    : std::integral_constant<bool, std::is_same<T, First>::value || is_in_list<T, Rest...>::value> {};


template <typename... Ts>
struct type_list {
    template <typename T>
    struct contains : is_in_list<T, Ts...> {};
};

template <typename T>
struct smart_ptr_comparator {
    bool operator()(const std::shared_ptr<T>& spl, const std::shared_ptr<T>& spr) const {
        return *spl < *spr;
    }

    bool operator()(const std::weak_ptr<T>& wpl, const std::weak_ptr<T>& wpr) const {
        auto spl = wpl.lock();
        auto spr = wpr.lock();
        if (spl && spr) {
            return *spl < *spr;
        }
        return false;
    }
};


template <typename UpperTypeList, typename LowerTypeList>
struct layer;

template <typename... UpperTypes, typename... LowerTypes>
struct layer<type_list<UpperTypes...>, type_list<LowerTypes...>> {
    using upper_list = type_list<UpperTypes...>;
    using lower_list = type_list<LowerTypes...>;

    // clang-format off
    using upper_layer = typename boost::fusion::map<
        boost::fusion::pair<
            UpperTypes,
            std::set<std::weak_ptr<UpperTypes>, smart_ptr_comparator<UpperTypes>>
        >...
    >;
    using lower_layer = typename boost::fusion::map<
        boost::fusion::pair<
            LowerTypes,
            std::set<std::shared_ptr<LowerTypes>, smart_ptr_comparator<LowerTypes>>
        >...
    >;
    // clang-format on

    upper_layer upper;
    lower_layer lower;

    template <typename T>
    void add_upper(std::weak_ptr<T> wp) {
        static_assert(upper_list::template contains<T>::value, "Type not in UpperTypes");
        auto& container = boost::fusion::at_key<T>(upper);
        auto inserter = std::inserter(container, container.end());
        inserter = wp;
    }

    template <typename T>
    void add_lower(std::shared_ptr<T> sp) {
        static_assert(lower_list::template contains<T>::value, "Type not in LowerTypes");
        auto& container = boost::fusion::at_key<T>(lower);
        auto inserter = std::inserter(container, container.end());
        inserter = sp;
    }

    void prune_expired() {
        using unused_t = int[];
        unused_t unused_upper{(prune_upper<UpperTypes>(), 0)...};
        unused_t unused_lower{(prune_lower<LowerTypes>(), 0)...};
    }

    template <typename T>
    std::shared_ptr<T> get_upper(std::weak_ptr<T> wp) {
        static_assert(upper_list::template contains<T>::value, "Type not in UpperTypes");
        auto sp = wp.lock();
        if (sp) {
            return sp;
        }
        upper_layer::template at_key<T>(upper).erase(wp);
    }

   private:
    template <typename T>
    void prune_upper() {
        auto& set = boost::fusion::at_key<T>(upper);
        for (auto it = set.begin(); it != set.end();) {
            if (it->expired())
                it = set.erase(it);
            else
                ++it;
        }
    }

    template <typename T>
    void prune_lower() {}
};
}  // namespace layers
}  // namespace ltz