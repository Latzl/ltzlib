#pragma once
#include <boost/fusion/include/map.hpp>
#include <boost/fusion/include/at_key.hpp>

namespace ltz {
namespace layers {
namespace _detail {

template <template <typename> class Set, typename... Types>
struct basic_sets {
    using sets = boost::fusion::map<boost::fusion::pair<Types, Set<Types>>...>;

   protected:
    sets sets_;

   private:
    template <typename T, typename... Us>
    struct is_in_list : std::false_type {};

    template <typename T, typename First, typename... Rest>
    struct is_in_list<T, First, Rest...>
        : std::integral_constant<bool, std::is_same<T, First>::value || is_in_list<T, Rest...>::value> {};

   public:
    template <typename T>
    struct contain : is_in_list<T, Types...> {};

   public:
    template <typename T>
    auto &get_set() const{
        static_assert(contain<T>::value, "type not match");
        return boost::fusion::at_key<T>(sets_);
    }

    template <typename T>
    auto &get_set() {
        static_assert(contain<T>::value, "type not match");
        return boost::fusion::at_key<T>(sets_);
    }


    template <typename Type, typename T>
    bool add(T &&t) {
        static_assert(contain<Type>::value, "type not match");
        return get_set<Type>().insert(std::forward<T>(t)).second;
    }
};

}  // namespace _detail
}  // namespace layers
}  // namespace ltz