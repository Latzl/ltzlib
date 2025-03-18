#include "common.hpp"
#include <ltz/layers/layers.hpp>

#define FN(...) FN_LAYERS(misc, __VA_ARGS__)
#define DF(desc, ...) DF_LAYERS(desc, misc, __VA_ARGS__)

namespace fn_upper {
using namespace ltz::layers::upper;
FN(upper, 0) {
    using upper_set = sets<int, double, std::string>;
    upper_set sets;

    auto sp3 = std::make_shared<int>(3);
    sets.add(sp3);
    auto sp4 = std::make_shared<int>(2);
    sets.add(sp4);

    std::shared_ptr<double> sp = std::make_shared<double>(3.14);
    sets.add(std::weak_ptr<double>(sp));
    std::shared_ptr<double> sp1 = std::make_shared<double>(3.145);
    sets.add(std::weak_ptr<double>(sp1));
    std::shared_ptr<double> sp2 = std::make_shared<double>(3.0);
    sets.add(std::weak_ptr<double>(sp2));

    auto sp5 = std::make_shared<std::string>("hello");
    sets.add(sp5);
    auto sp6 = std::make_shared<std::string>("hello world");
    sets.add(sp6);

    auto print_upper_info = [&sets](const auto& t) {
        using type = typename std::decay<decltype(t)>::type;
        // using type = int;
        auto& set = sets.get_set<type>();
        fmt::print("set.size(): {}\n", set.size());
        sets.for_each<type>([&t](auto sp) {
            //
            fmt::print("upper {} value: {}\n", typeid(t).name(), *sp);
        });
    };

    print_upper_info((int)1);
    print_upper_info((double)1.0);
    print_upper_info((std::string) "1");


    return 0;
}
}  // namespace fn_upper

namespace fn_lower {
using namespace ltz::layers::lower;
FN(lower, 0) {
    using Sets = sets<int, double, std::string>;
    Sets sets;

    auto sp3 = std::make_shared<int>(3);
    sets.add(sp3);
    auto sp4 = std::make_shared<int>(2);
    sets.add(sp4);

    sets.make<int>(1);
    sets.make<int>(2);
    sets.make<int>(3);

    sets.make<double>(1.0);
    sets.make<double>(2.0);
    sets.make<double>(3.0);

    sets.make<std::string>("hello");
    sets.make<std::string>("hello world");
    sets.make<std::string>("hello world 2");

    auto print_lower_info = [&sets](const auto& t) {
        using type = typename std::decay<decltype(t)>::type;
        // using type = int;
        auto& set = sets.get_set<type>();
        fmt::print("set.size(): {}\n", set.size());
        for (const auto& sp : set) {
            if (sp) {
                fmt::print("upper {} value: {}\n", typeid(t).name(), *sp);
            }
        }
    };

    print_lower_info((int)1);
    print_lower_info((double)1.0);
    print_lower_info((std::string) "1");


    return 0;
}
}  // namespace fn_lower


namespace fn_layer {
using namespace ltz::layers;
FN(layer, 0) {
    using upper_sets = upper::sets<int, double, std::string>;
    using lower_sets = lower::sets<std::string, char>;

    layer<upper_sets, lower_sets> lyr;


    auto print_upper_info = [&lyr](const auto& t) {
        using type = typename std::decay<decltype(t)>::type;
        // using type = int;
        auto& set = lyr.upper.get_set<type>();
        fmt::print("set.size(): {}\n", set.size());

        lyr.upper.for_each<type>([&t](auto sp) {
            //
            fmt::print("upper {} value: {}\n", typeid(t).name(), *sp);
        });
    };
    {
        auto sp3 = std::make_shared<int>(3);
        lyr.upper.add(sp3);
        auto sp4 = std::make_shared<int>(2);
        lyr.upper.add(sp4);

        std::shared_ptr<double> sp = std::make_shared<double>(3.14);
        lyr.upper.add(std::weak_ptr<double>(sp));
        std::shared_ptr<double> sp1 = std::make_shared<double>(3.145);
        lyr.upper.add(std::weak_ptr<double>(sp1));
        std::shared_ptr<double> sp2 = std::make_shared<double>(3.0);
        lyr.upper.add(std::weak_ptr<double>(sp2));

        auto sp5 = std::make_shared<std::string>("hello");
        lyr.upper.add(sp5);
        auto sp6 = std::make_shared<std::string>("hello world");
        lyr.upper.add(sp6);


        print_upper_info((int)1);
        print_upper_info((double)1.0);
        print_upper_info((std::string) "1");
    }
    fmt::print("===\n");
    print_upper_info((int)1);
    print_upper_info((double)1.0);
    print_upper_info((std::string) "1");
    fmt::print("===\n");

    auto print_lower_info = [&lyr](const auto& t) {
        using type = typename std::decay<decltype(t)>::type;
        // using type = int;
        auto& set = lyr.lower.get_set<type>();
        fmt::print("set.size(): {}\n", set.size());
        for (const auto& sp : set) {
            if (sp) {
                fmt::print("lower {} value: {}\n", typeid(t).name(), *sp);
            }
        }
    };
    {
        lyr.lower.make<std::string>("string");
        lyr.lower.make<std::string>("string this");
        lyr.lower.make<std::string>("string this ga");
        lyr.lower.make<char>('a');
        lyr.lower.make<char>('b');
        auto r = lyr.lower.make<char>('c');
        if (r) {
            fmt::print("r: {}\n", *r);
        } else {
            fmt::print("r: nullptr\n");
        }
        lyr.lower.make<char>('d');

        print_lower_info((std::string) "1");
        print_lower_info((char)1);
    }
    fmt::print("===\n");
    print_lower_info((std::string) "1");
    print_lower_info((char)1);

    return 0;
}

}  // namespace fn_layer

#include <boost/mp11.hpp>

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