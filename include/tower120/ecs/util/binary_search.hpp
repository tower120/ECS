#pragma once

#include <range/v3/algorithm/lower_bound.hpp>
#include <functional>
#include <optional>

namespace tower120::ecs::util{

    // implementation from https://en.cppreference.com/w/cpp/algorithm/binary_search
    template<typename I, typename S, typename V, typename C = ranges::less, typename P = ranges::identity>
    std::optional<I> binary_search(I begin, S end, V const &val, C pred = C{}, P proj = P{}){
        begin = ranges::lower_bound(begin, end, val, pred, proj);
        if (!(begin == end) && !(pred(val, proj(*begin)))){
            return {begin};
        } else {
            return {};
        }
    }

}