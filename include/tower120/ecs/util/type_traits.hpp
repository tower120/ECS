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
        constexpr void static_for(std::index_sequence<Is...>, Fn&& fn){
            ( fn( std::integral_constant<std::size_t, Is>{} ), ... );
        }
    }
    template<std::size_t N, class Fn>
    constexpr void static_for(Fn&& fn){
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

    // all_unique
    template <typename...>
    constexpr bool all_unique = true;

    template <typename T, typename... Rest>
    constexpr bool all_unique<T, Rest...> =
        (!std::is_same_v<T, Rest> && ...) && all_unique<Rest...>;

    // all_same
    template <typename...>
    constexpr bool all_same = true;

    template <typename T, typename... Rest>
    constexpr bool all_same<T, Rest...> =
        (std::is_same_v<T, Rest> && ...) && all_same<Rest...>;


    // tuple_array
    namespace detail{
        template<class T, std::size_t>
        using ident_t = T;

        template<class T, template<class...> class Tuple, class S>
        struct tuple_array;

        template<class T, template<class...> class Tuple, std::size_t ...Is>
        struct tuple_array<T, Tuple, std::index_sequence<Is...>>{
            using type = Tuple<detail::ident_t<T, Is>...>;
        };
    }
    template<class T, std::size_t N, template<class...> class Tuple = std::tuple>
    using tuple_array = typename detail::tuple_array<T, Tuple, std::make_index_sequence<N>>::type;

    // is_tuple_array
    namespace detail{
        template<class Tuple>
        struct is_tuple_array {
            static constexpr bool value = false;
        };
        template<class... Args>
        struct is_tuple_array< std::tuple<Args...> > {
            static constexpr bool value = all_same<Args...>;
        };
    }
    template <typename Tuple>
    constexpr bool is_tuple_array = detail::is_tuple_array<Tuple>::value;

    // tuple_array_accumulate
    template<class TupleArray, class T, class Op>
    constexpr T tuple_array_accumulate(TupleArray& tuple_array, T init, Op&& op){
        static_assert(is_tuple_array<TupleArray>);

        static_for<std::tuple_size_v<TupleArray>>([&](auto n){
            init = op(std::move(init), std::get<n.value>(tuple_array));
        });

        return init;
    }

    // tuple_array_max
    /*template<class TupleArray, class Proj = ranges::ident>
    constexpr auto tuple_array_max(TupleArray& tuple_array, Proj proj = Proj{}){
        static_assert(is_tuple_array<TupleArray>);
        static_assert(std::tuple_size_v<TupleArray> > 0 );

        using T = std::tuple_element_t<0, TupleArray>;
        T max   = std::get<0>(tuple_array);

        static_for<std::tuple_size_v<TupleArray>>([&](auto n){
            if constexpr (n.value == 0) return; // skip first

            constexpr const T& value = std::get<n.value>(tuple_array);
            if (proj(value) > proj(max)) max = value;
        });

        return max;
    }*/
}