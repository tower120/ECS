#pragma once

#include <cstddef>
#include <type_traits>
#include <algorithm>
#include <array>
#include <typeindex>
#include <cassert>

namespace tower120::ecs::util{
    namespace detail::heterogeneous_array{
        constexpr std::size_t apply_alignment(const std::size_t pos, const std::size_t alignemnt){
            if (pos % alignemnt == 0){
                return pos;
            } else {
                return (pos / alignemnt) * alignemnt + alignemnt;
            }
        }

        template<int N, typename... Ts>
        using NthTypeOf = typename std::tuple_element<N, std::tuple<Ts...>>::type;

        template <int N, int I /*=0*/, class ...Elements>
        constexpr std::size_t get_begin_(const std::size_t pos = 0){
            if constexpr (I==N){
                return pos;
            } else {
                using Element = NthTypeOf<I, Elements...>;

                std::size_t next_pos = pos + sizeof(Element);

                if constexpr (I < sizeof...(Elements)-1){
                    using NextElement = NthTypeOf<I+1, Elements...>;
                    next_pos = apply_alignment(next_pos, alignof(NextElement) );
                }

                return get_begin_<N, I+1, Elements...>( next_pos );
            }
        }

        template <int N, class ...Elements>
        constexpr std::size_t get_begin(){
            return get_begin_<N, 0, Elements...>();
        }
    }  // namespace util::heterogeneous_array


    template <class ...Elements>
    class heterogeneous_array{
    public:
        using offset_table_t = std::array<std::size_t, sizeof...(Elements)>;
        using typeindex_table_t = std::array<std::type_index, sizeof...(Elements)>;

    private:
        static const constexpr std::size_t last_element_id = sizeof...(Elements)-1;
        using LastElement = detail::heterogeneous_array::NthTypeOf<last_element_id, Elements...>;
        static const constexpr std::size_t max_align = std::max( {alignof(Elements)...} );

        template<std::size_t ...I>
        static constexpr offset_table_t get_offset_table(std::index_sequence<I...>){
            return {
                detail::heterogeneous_array::get_begin<
                    I,
                    Elements...
                >()...
            };
        }
    public:
        static const constexpr offset_table_t offset_table = get_offset_table(std::index_sequence_for<Elements...>{});

        inline static const typeindex_table_t typeindex_table {
            std::type_index(typeid(Elements))...
        };

        static const constexpr std::size_t storage_size =
            detail::heterogeneous_array::apply_alignment(
                offset_table[last_element_id] + sizeof(LastElement),
                max_align
            );

    private:
        std::byte storage[storage_size];

        template<int I>
        constexpr void construct() noexcept {
            using Element = detail::heterogeneous_array::NthTypeOf<I, Elements...>;
            if constexpr (!std::is_trivially_default_constructible_v<Element>){
                void* address = &storage[offset_table[I]];
                new (reinterpret_cast<Element*>(address)) Element ();
            }
        }

        template<std::size_t ...I>
        constexpr void default_construct(std::index_sequence<I...>) noexcept {
            (construct<I>(), ...);
        }

        void foreach_container(){

        }

    public:
        constexpr heterogeneous_array() noexcept
        {
            default_construct(std::index_sequence_for<Elements...>{});
        }

        // TODO : non-default ctr

        template<class T>
        constexpr T& get(std::size_t index){
            assert(typeindex_table[index] == typeid(T));

            const std::size_t offset = offset_table[index];
            void* address = &storage[offset];
            return *reinterpret_cast<T*>(address);
        }
    };

}