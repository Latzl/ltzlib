#pragma once
#include <boost/mp11.hpp>
#include <memory>

namespace ltz {
namespace layers {
namespace single_chain {

template <typename T>
struct is_node;

template <typename Derived, typename UpperType, typename LowerType>
class node : public std::enable_shared_from_this<Derived> {
   public:
    using upper_type = UpperType;
    using lower_type = LowerType;

   public:
    std::weak_ptr<upper_type> upper;
    std::shared_ptr<lower_type> lower;

    node() = default;
    node(std::weak_ptr<upper_type> wp) : upper(wp) {}

   public:
    /**
       @brief check if T has upper type
    */
    using has_upper = std::integral_constant<bool, !std::is_same<upper_type, void>::value>;
    using has_lower = std::integral_constant<bool, !std::is_same<lower_type, void>::value>;

   private:
    template <typename Upper, bool HasUpper = has_upper::value>
    struct get_uppers_impl;
    template <typename Upper>
    struct get_uppers_impl<Upper, true> {
        using type = boost::mp11::mp_push_front<typename Upper::get_uppers::type, Upper>;
    };
    template <typename Upper>
    struct get_uppers_impl<Upper, false> {
        using type = boost::mp11::mp_list<>;
    };

    template <typename Lower, bool HasLower = has_lower::value>
    struct get_lowers_impl;
    template <typename Lower>
    struct get_lowers_impl<Lower, true> {
        using type = boost::mp11::mp_push_front<typename Lower::get_lowers::type, Lower>;
    };
    template <typename Lower>
    struct get_lowers_impl<Lower, false> {
        using type = boost::mp11::mp_list<>;
    };

   public:
    /**
        @brief get upper types
        
     */
    struct get_uppers {
        using type = typename get_uppers_impl<upper_type>::type;
    };
    /**
        @brief get lower types
        
     */
    struct get_lowers {
        using type = typename get_lowers_impl<lower_type, has_lower::value>::type;
    };

   public:
    /**
        @brief check if this type is upper of T
        
     */
    template <typename T>
    struct is_upper_of {
        using list = typename get_lowers::type;
        static constexpr bool value = boost::mp11::mp_contains<list, T>::value;
    };

    /**
        @brief 
        
     */
    template <typename T>
    struct is_lower_of {
        using list = typename get_uppers::type;
        static constexpr bool value = boost::mp11::mp_contains<list, T>::value;
    };

    /**
        @brief get zero-based index of upper type in uppers
        
        @tparam T 
     */
    template <typename T>
    struct upper_index {
       private:
        using uppers = typename get_uppers::type;
        static_assert(is_lower_of<T>::value, "T is not an upper type of this node");
        static constexpr std::size_t pos = boost::mp11::mp_find<uppers, T>::value;
        static_assert(pos != boost::mp11::mp_size<uppers>::value, "T not found in the upper list");

       public:
        static constexpr std::size_t value{pos};
    };

    /**
        @brief get zero-based index of lower type in lowers
        
        @tparam T 
     */
    template <typename T>
    struct lower_index {
       private:
        using lowers = typename get_lowers::type;
        static_assert(is_upper_of<T>::value, "T is not an lower type of this node");
        static constexpr std::size_t pos = boost::mp11::mp_find<lowers, T>::value;
        static_assert(pos != boost::mp11::mp_size<lowers>::value, "T not found in the lower list");

       public:
        static constexpr std::size_t value{pos};
    };

    /**
        @brief make this dirived type
        
        @tparam Ts 
        @param args 
        @return std::shared_ptr<Derived> 
     */
    template <typename... Ts>
    static std::shared_ptr<Derived> make_shared(Ts &&...args) {
        return std::make_shared<Derived>(std::forward<Ts>(args)...);
    }

    /**
        @brief make lower
        
        @tparam Ts 
        @param args 
        @return std::shared_ptr<lower_type> 
     */
    template <typename... Ts>
    std::shared_ptr<lower_type> make_lower(Ts &&...args) {
        lower = std::make_shared<lower_type>(this->shared_from_this(), std::forward<Ts>(args)...);
        return lower;
    }

    /**
        @brief Get the upper smart pointer object
        
        @tparam T 
        @tparam std::enable_if<!std::is_same<T, upper_type>::value>::type 
        @return std::shared_ptr<T> 
     */
    template <typename T, typename std::enable_if<!std::is_same<T, upper_type>::value>::type * = nullptr>
    std::shared_ptr<T> get_upper() {
        static_assert(is_node<T>::value, "T is not a node type");
        auto sp = upper.lock();
        if (!sp) {
            return nullptr;
        }
        return sp->template get_upper<T>();
    }

    template <typename T, typename std::enable_if<std::is_same<T, upper_type>::value>::type * = nullptr>
    std::shared_ptr<T> get_upper() {
        return upper.lock();
    }

    /**
        @brief Get the lower smart pointer object
        
        @tparam T 
        @tparam std::enable_if<!std::is_same<T, lower_type>::value>::type 
        @return std::shared_ptr<T> 
     */
    template <typename T, typename std::enable_if<!std::is_same<T, lower_type>::value>::type * = nullptr>
    std::shared_ptr<T> get_lower() {
        static_assert(is_node<T>::value, "T is not a node type");
        return lower->template get_lower<T>();
    }

    template <typename T, typename std::enable_if<std::is_same<T, lower_type>::value>::type * = nullptr>
    std::shared_ptr<T> get_lower() {
        return lower;
    }

    /**
        @brief get upper or lower smart pointer object
        
        @tparam T 
        @tparam std::enable_if<is_lower_of<T>::value>::type 
        @return std::shared_ptr<T> 
     */
    template<typename T, typename std::enable_if<is_lower_of<T>::value>::type * = nullptr>
    std::shared_ptr<T> get() {
        return get_upper<T>();
    }

    template<typename T, typename std::enable_if<is_upper_of<T>::value>::type * = nullptr>
    std::shared_ptr<T> get() {
        return get_lower<T>();
    }
};

template <typename T>
struct is_node {
   private:
    template <typename D, typename U, typename L>
    static constexpr std::true_type check(const node<D, U, L> *);

    static constexpr std::false_type check(...);

   public:
    static constexpr bool value = decltype(check(std::declval<T *>()))::value;
};
}  // namespace single_chain
}  // namespace layers
}  // namespace ltz