#include "common.hpp"
#include <ltz/layers/layers.hpp>

#define GF(...) GF_LAYERS(__VA_ARGS__)

using namespace ltz::layers;
GF(normal) { 
    using upper_sets = upper::sets<int, std::string>;
    using lower_sets = lower::sets<char, double>;

    static_assert(upper_sets::contain<int>::value, "");
    static_assert(upper_sets::contain<std::string>::value, "");
    static_assert(!upper_sets::contain<double>::value, "");
    static_assert(lower_sets::contain<char>::value, "");
    static_assert(lower_sets::contain<double>::value, "");
    static_assert(!lower_sets::contain<int>::value, "");

    std::set<int> int_set{1, 2, 3};
    std::set<std::string> str_set{"sa", "sb", "sc"};

    std::set<char> char_set{'a', 'b', 'c'};
    std::set<double> double_set{1.1, 2.2, 3.3};

    layer<upper_sets, lower_sets> layer;

    auto update_ptr_set = [](const auto &origin_set, auto &ptr_set) {
        for (auto &item : origin_set) {
            ptr_set.insert(std::make_shared<typename std::decay<decltype(item)>::type>(item));
        }
    };

    {  // upper
        {
            std::set<std::shared_ptr<int>> int_ptr_set;
            std::set<std::shared_ptr<std::string>> str_ptr_set;

            update_ptr_set(int_set, int_ptr_set);
            update_ptr_set(str_set, str_ptr_set);

            for (auto &elem : int_ptr_set) {
                layer.upper.add(elem);
            }
            for (auto &elem : str_ptr_set) {
                layer.upper.add(elem);
            }
            ASSERT_EQ(layer.upper.get_set<int>().size(), 3);
            ASSERT_EQ(layer.upper.get_set<std::string>().size(), 3);

            {
                std::set<std::shared_ptr<int>> int_ptr_set2;
                layer.upper.for_each<int>([&int_ptr_set2](auto sp) { int_ptr_set2.insert(sp); });
                ASSERT_EQ(int_ptr_set2.size(), int_ptr_set.size());
                for (auto it = int_ptr_set.begin(), it2 = int_ptr_set2.begin(); it != int_ptr_set.end(); ++it, ++it2) {
                    EXPECT_EQ(*it, *it2);
                }

                std::set<std::shared_ptr<std::string>> str_ptr_set2;
                layer.upper.for_each<std::string>([&str_ptr_set2](auto sp) { str_ptr_set2.insert(sp); });
                ASSERT_EQ(str_ptr_set2.size(), str_ptr_set.size());
                for (auto it = str_ptr_set.begin(), it2 = str_ptr_set2.begin(); it != str_ptr_set.end(); ++it, ++it2) {
                    EXPECT_EQ(*it, *it2);
                }
            }
        }
        layer.upper.prune_expired();
        EXPECT_EQ(layer.upper.get_set<int>().size(), 0);
        EXPECT_EQ(layer.upper.get_set<std::string>().size(), 0);
    }

    {  // lower
        for (auto &elem : char_set) {
            layer.lower.make<char>(elem);
        }
        for (auto &elem : double_set) {
            layer.lower.make<double>(elem);
        }

        ASSERT_EQ(layer.lower.get_set<char>().size(), 3);
        ASSERT_EQ(layer.lower.get_set<double>().size(), 3);

        {
            std::set<char> char_set2;
            for (auto &elem : layer.lower.get_set<char>()) {
                char_set2.insert(*elem);
            }
            ASSERT_EQ(char_set2.size(), char_set.size());
            for (auto it = char_set.begin(), it2 = char_set2.begin(); it != char_set.end(); ++it, ++it2) {
                EXPECT_EQ(*it, *it2);
            }

            std::set<double> double_set2;
            for (auto &elem : layer.lower.get_set<double>()) {
                double_set2.insert(*elem);
            }
            ASSERT_EQ(double_set2.size(), double_set.size());
            for (auto it = double_set.begin(), it2 = double_set2.begin(); it != double_set.end(); ++it, ++it2) {
                EXPECT_EQ(*it, *it2);
            }
        }
    }
}

namespace gf_chain {

struct A;
struct B;
struct C;

struct A {
    int a{1};

    using upper_sets = upper::sets<void>;
    using lower_sets = lower::sets<B>;

    layer<upper_sets, lower_sets> lyr;
};

struct B {
    B(std::weak_ptr<A> wp) {
        lyr.upper.add(wp);
    }
    int b{2};

    using upper_sets = upper::sets<A>;
    using lower_sets = lower::sets<C>;

    layer<upper_sets, lower_sets> lyr;
};

struct C {
    C(std::weak_ptr<B> wp) {
        lyr.upper.add(wp);
    }
    int c{3};

    using upper_sets = upper::sets<B>;
    using lower_sets = lower::sets<void>;
    layer<upper_sets, lower_sets> lyr;
};

GF(chain, use_raw_layer) {
    std::shared_ptr<C> g_c;

    {
        auto a = std::make_shared<A>();
        a->lyr.lower.make<B>(a);

        auto b = *a->lyr.lower.get_set<B>().begin();
        EXPECT_EQ(b->b, 2);
        b->lyr.lower.make<C>(b);

        auto c = *b->lyr.lower.get_set<C>().begin();
        g_c = c;

        EXPECT_EQ(c->c, 3);
        auto a2 = c->lyr.upper.get_set<B>().begin()->lock()->lyr.upper.get_set<A>().begin()->lock();
        EXPECT_EQ(a2->a, 1);
    }
    {
        // as b destroyed
        int cnt = 0;
        g_c->lyr.upper.for_each<B>([&cnt](auto sp) { cnt++; });
        EXPECT_EQ(cnt, 0);
        EXPECT_EQ(g_c->lyr.upper.get_set<B>().size(), 0);
    }
}

}  // namespace gf_chain
