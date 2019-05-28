#pragma once

#include <tuple>
#include <variant>

namespace tower120::ecs::util {
    // type_wrapper
    template<class T>
    struct type_wrapper{
        using type = T;
    };

    // static_for
    namespace detail{
        template<std::size_t ...Is, class Fn>
        void static_for(std::index_sequence<Is...>, Fn&& fn){
            ( fn( std::integral_constant<std::size_t, Is>{} ), ... );
        }
    }
    template<std::size_t N, class Fn>
    void static_for(Fn&& fn){
        detail::static_for( std::make_index_sequence<N>{}, std::forward<Fn>(fn) );
    }

    // TypeN
    template<std::size_t N, typename... Ts>
    using TypeN = typename std::tuple_element<N, std::tuple<Ts...>>::type;

    // has_type
    template<typename T, typename... Ts>
    constexpr bool has_type = (std::is_same_v<T, Ts> || ...);

    // type_index
    namespace detail{
        template<typename R, typename T, typename... Ts, int I = 0>
        constexpr int type_index (std::integral_constant<int, I> = {}){
            if constexpr (std::is_same_v<R, T>){
                return I;
            } else {
                static_assert(sizeof...(Ts) > 0, "Type not found.");
                return type_index<R, Ts...>(std::integral_constant<int, I+1>{});
            }
        }
    }
    template<typename T, typename... Ts>
    constexpr int type_index = detail::type_index<T, Ts...>();

    // is_unique
    template <typename...>
    constexpr bool is_unique = true;

    template <typename T, typename... Rest>
    constexpr bool is_unique<T, Rest...> =
        (!std::is_same_v<T, Rest> && ...) && is_unique<Rest...>;
}