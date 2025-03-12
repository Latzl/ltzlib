#include "common.hpp"
#include <ltz/layers/layer.hpp>

#define FN(...) FN_LAYERS(misc, __VA_ARGS__)
#define DF(desc, ...) DF_LAYERS(desc, misc, __VA_ARGS__)
#define GF(...) GF_LAYERS(misc, __VA_ARGS__)

using namespace ltz::layers;
using MyUpperTypes = type_list<double, std::string>;
using MyLowerTypes = type_list<int, double, std::string>;

using mylayer = layer<MyUpperTypes, MyLowerTypes>;

FN(0) {
    mylayer h;

    auto lowerInt = std::make_shared<int>(42);
    h.add_lower(lowerInt);

    std::shared_ptr<double> upperDouble = std::make_shared<double>(3.14);
    h.add_upper(std::weak_ptr<double>(upperDouble));

    h.prune_expired();

    auto& int_set = boost::fusion::at_key<int>(h.lower);
    fmt::print("int_set.size(): {}\n", int_set.size());

    for (const auto& sp : int_set) {
        if (sp) {
            std::cout << "Lower int value: " << *sp << std::endl;
        }
    }

    return 0;
}