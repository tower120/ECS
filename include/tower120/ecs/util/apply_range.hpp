#pragma once

#include <array>
#include <functional>

// NOT IN USE

namespace tower120::ecs::util{
    namespace detail::apply_range{
        template<typename Fn, typename Range, std::size_t ...Is>
        decltype(auto) apply_range(Fn&& fn, Range&& range, std::index_sequence<Is...>)
            // TODO: noexcept
        {
            return std::invoke(std::forward<Fn>(fn), range[Is]...);
        }
    }

    template<std::size_t N, typename Fn, typename Range>
    decltype(auto) apply_range(Fn&& fn, Range&& range)
        // TODO: noexcept
    {
        return detail::apply_range::apply_range(
            std::forward<Fn>(fn), std::forward<Range>(range), std::make_index_sequence<N>{}
        );
    }
    template<typename Fn, typename Range>
    decltype(auto) apply_range(Fn&& fn, Range&& range)
        // TODO: noexcept
    {
        constexpr const std::size_t N = ranges::range_cardinality<Range>::value;
        return detail::apply_range::apply_range(
            std::forward<Fn>(fn), std::forward<Range>(range), std::make_index_sequence<N>{}
        );
    }
}