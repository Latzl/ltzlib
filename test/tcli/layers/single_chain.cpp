#include "common.hpp"
#include <ltz/layers/layers.hpp>

#define FN(...) FN_LAYERS(single_chain, __VA_ARGS__)
#define DF(desc, ...) DF_LAYERS(desc, single_chain, __VA_ARGS__)

namespace fn_single_chain {

using namespace ltz::layers::single_chain;

struct A;
struct B;
struct C;

struct A : public node<A, void, B> {
    int a{1};
};

struct B : public node<B, A, C> {
    using node = node<B, A, C>;
    B(std::weak_ptr<A> wp, int b) : node(wp), b(b) {}
    int b{2};
};

struct C : public node<C, B, void> {
    C(std::weak_ptr<B> wp, int c) : node(wp), c(c) {}
    int c{3};
};

static_assert(is_node<A>::value, "");
static_assert(is_node<B>::value, "");
static_assert(is_node<C>::value, "");
static_assert(!is_node<void>::value, "");
static_assert(!is_node<int>::value, "");
static_assert(!is_node<char>::value, "");

static_assert(!A::has_upper::value, "");
static_assert(B::has_upper::value, "");
static_assert(C::has_upper::value, "");
static_assert(std::is_same<A::get_uppers::type, boost::mp11::mp_list<>>::value, "");
static_assert(std::is_same<B::get_uppers::type, boost::mp11::mp_list<A>>::value, "");
static_assert(std::is_same<C::get_uppers::type, boost::mp11::mp_list<B, A>>::value, "");
static_assert(!A::is_upper_of<A>::value, "");
static_assert(A::is_upper_of<B>::value, "");
static_assert(A::is_upper_of<C>::value, "");
static_assert(!B::is_upper_of<A>::value, "");
static_assert(!B::is_upper_of<B>::value, "");
static_assert(B::is_upper_of<C>::value, "");
static_assert(!C::is_upper_of<A>::value, "");
static_assert(!C::is_upper_of<B>::value, "");
static_assert(!C::is_upper_of<C>::value, "");

static_assert(A::has_lower::value, "");
static_assert(B::has_lower::value, "");
static_assert(!C::has_lower::value, "");
static_assert(std::is_same<A::get_lowers::type, boost::mp11::mp_list<B, C>>::value, "");
static_assert(std::is_same<B::get_lowers::type, boost::mp11::mp_list<C>>::value, "");
static_assert(std::is_same<C::get_lowers::type, boost::mp11::mp_list<>>::value, "");
static_assert(!A::is_lower_of<A>::value, "");
static_assert(!A::is_lower_of<B>::value, "");
static_assert(!A::is_lower_of<C>::value, "");
static_assert(B::is_lower_of<A>::value, "");
static_assert(!B::is_lower_of<B>::value, "");
static_assert(!B::is_lower_of<C>::value, "");
static_assert(C::is_lower_of<A>::value, "");
static_assert(C::is_lower_of<B>::value, "");
static_assert(!C::is_lower_of<C>::value, "");

FN(single_chain, 0) {
    auto a = A::make_shared();
    auto c = a->make_lower(-2)->make_lower(-3);
    fmt::print("{}\n", c->c);

    auto a_got = c->get_upper<A>();
    fmt::print("{}\n", (bool)a_got);
    if (a_got) {
        fmt::print("{}\n", a_got->a);
    }

    auto c_got = a->get_lower<C>();
    fmt::print("{}\n", (bool)c_got);
    if (c_got) {
        fmt::print("{}\n", c_got->c);
    }

    auto b = a->lower;
    auto a_got2 = b->get<A>();
    auto c_got2 = b->get<C>();
    fmt::print("{}\n", (bool)a_got2);
    fmt::print("{}\n", (bool)c_got2);
    if (a_got2) {
        fmt::print("{}\n", a_got2->a);
    }
    if (c_got2) {
        fmt::print("{}\n", c_got2->c);
    }

    return 0;
}

}  // namespace fn_single_chain