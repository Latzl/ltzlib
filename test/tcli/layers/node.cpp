#include "common.hpp"
#include "ltz/layers/layer/node.hpp"
#include <ltz/layers/layers.hpp>

#define FN(...) FN_LAYERS(node, __VA_ARGS__)
#define DF(desc, ...) DF_LAYERS(desc, node, __VA_ARGS__)

namespace fn_node {

using namespace ltz::layers;
struct A;
struct B;
struct C;
struct D;
struct E;
/* 
     +---A
     ↓   !
     B   !
     |   !
     +-->C
         |
    +----+--+
    |       |
    ↓       ↓
    D       E
 */

struct A : public node<A, upper::sets<void>, lower::sets<B>> {
    A(int a) : a(a) {}
    int a{1};
};

struct B : public node<B, upper::sets<A>, lower::sets<C>> {
    B(std::weak_ptr<A> wp) : node(wp) {}
    int b{2};
};

struct C : public node<C, upper::sets<B, A>, lower::sets<D, E>> {
    C(std::weak_ptr<B> wp, int c) : node(wp), c(c) {}
    int c{3};
};

struct D : public node<D, upper::sets<C>, lower::sets<void>> {
    D(std::weak_ptr<C> wp) : node(wp) {}
    int d{4};
};

struct E : public node<E, upper::sets<C>, lower::sets<>> {
    E(std::weak_ptr<C> wp) : node(wp) {}
    int e{5};
};

static_assert(A::upper_sets::contain<void>::value, "");
static_assert(!A::upper_sets::contain<int>::value, "");
static_assert(A::lower_sets::contain<B>::value, "");
static_assert(!A::lower_sets::contain<C>::value, "");
static_assert(B::upper_sets::contain<A>::value, "");
static_assert(B::lower_sets::contain<C>::value, "");
static_assert(C::upper_sets::contain<B>::value, "");
static_assert(C::upper_sets::contain<A>::value, "");
static_assert(C::lower_sets::contain<D>::value, "");
static_assert(C::lower_sets::contain<E>::value, "");
static_assert(D::upper_sets::contain<C>::value, "");
static_assert(E::upper_sets::contain<C>::value, "");

FN(0) {
    auto a = A::make_shared(-1);
    auto b = a->make_lower<B>();
    auto c = b->make_lower<C>(-3);
    c->layer.upper.add(a);
    auto d = c->make_lower<D>();
    auto e = c->make_lower<E>();
    {
        fmt::print("a: {}, ref: {}\n", (void *)a.get(), a.use_count());
        fmt::print("b: {}, ref: {}\n", (void *)b.get(), b.use_count());
        fmt::print("c: {}, ref: {}\n", (void *)c.get(), c.use_count());
        fmt::print("d: {}, ref: {}\n", (void *)d.get(), d.use_count());
        fmt::print("e: {}, ref: {}\n", (void *)e.get(), e.use_count());
    }

    while (true) {  // b
        fmt::print(">>> b\n");
        auto &upper_set = b->layer.upper.get_set<A>();
        fmt::print("upper_set.size: {}\n", upper_set.size());
        auto a_got = upper_set.begin()->lock();
        if (!a_got) {
            fmt::print(stderr, "a_got is null\n");
            break;
        }
        fmt::print("a: {}, ref: {}\n", (void *)a_got.get(), a_got.use_count());

        break;
    }

    while (true) {  // c
        fmt::print(">>> c\n");
        while (true) {
            fmt::print(">>>> b\n");
            auto &upper_set = c->layer.upper.get_set<B>();
            fmt::print("upper_set.size: {}\n", upper_set.size());
            auto b_got = upper_set.begin()->lock();
            if (!b_got) {
                fmt::print(stderr, "b_got is null\n");
                break;
            }
            fmt::print("b: {}, ref: {}\n", (void *)b_got.get(), b_got.use_count());
            break;
        }

        while (true) {
            fmt::print(">>>> a\n");
            auto &upper_set = c->layer.upper.get_set<A>();
            fmt::print("upper_set.size: {}\n", upper_set.size());
            auto a_got = upper_set.begin()->lock();
            if (!a_got) {
                fmt::print(stderr, "a_got is null\n");
            }
            fmt::print("a: {}, ref: {}\n", (void *)a_got.get(), a_got.use_count());
            break;
        }

        break;
    }

    while (true) {  // d
        fmt::print(">>> d\n");

        auto &upper_set = d->layer.upper.get_set<C>();
        fmt::print("upper_set.size: {}\n", upper_set.size());
        auto c_got = upper_set.begin()->lock();
        if (!c_got) {
            fmt::print(stderr, "c_got is null\n");
            break;
        }
        fmt::print("c: {}, ref: {}\n", (void *)c_got.get(), c_got.use_count());

        break;
    }

    while (true) {  // e
        fmt::print(">>> e\n");
        auto &upper_set = e->layer.upper.get_set<C>();
        fmt::print("upper_set.size: {}\n", upper_set.size());
        auto c_got = upper_set.begin()->lock();
        if (!c_got) {
            fmt::print(stderr, "c_got is null\n");
        }
        fmt::print("c: {}, ref: {}\n", (void *)c_got.get(), c_got.use_count());
        break;
    }

    return 0;
}

}  // namespace fn_node