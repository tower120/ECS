#pragma once

#include <cstddef>
#include <type_traits>
#include <algorithm>
#include <array>
#include <typeindex>
#include <cassert>

#include "type_traits.hpp"

namespace tower120::ecs::util{
    namespace detail::heterogeneous_array{
        constexpr std::size_t apply_alignment(const std::size_t pos, const std::size_t alignemnt){
            if (pos % alignemnt == 0){
                return pos;
            } else {
                return (pos / alignemnt) * alignemnt + alignemnt;
            }
        }

        template <int N, int I /*=0*/, class ...Elements>
        constexpr std::size_t get_begin_(const std::size_t pos = 0){
            if constexpr (I==N){
                return pos;
            } else {
                using Element = TypeN<I, Elements...>;

                std::size_t next_pos = pos + sizeof(Element);

                if constexpr (I < sizeof...(Elements)-1){
                    using NextElement = TypeN<I+1, Elements...>;
                    next_pos = apply_alignment(next_pos, alignof(NextElement) );
                }

                return get_begin_<N, I+1, Elements...>( next_pos );
            }
        }

        template <int N, class ...Elements>
        constexpr std::size_t get_begin(){
            return get_begin_<N, 0, Elements...>();
        }

        template <class ...Elements>
        static const constexpr std::size_t max_align =  std::max( {alignof(Elements)...} );
    }  // namespace util::heterogeneous_array


    template <class ...Elements>
    class alignas(detail::heterogeneous_array::max_align<Elements...>) heterogeneous_array {
    public:
        using offset_table_t = std::array<std::size_t, sizeof...(Elements)>;
        using typeindex_table_t = std::array<std::type_index, sizeof...(Elements)>;
    private:
        template<int N>
        using ElementN = TypeN<N, Elements...>;

        static const constexpr std::size_t last_element_id = sizeof...(Elements)-1;
        using LastElement = TypeN<last_element_id, Elements...>;

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
    private:
        static const constexpr std::size_t storage_size =
            detail::heterogeneous_array::apply_alignment(
                offset_table[last_element_id] + sizeof(LastElement),
                detail::heterogeneous_array::max_align<Elements...>
            );
    public:
        using Storage = std::array<std::byte, storage_size>;
    private:
        Storage storage;

        template<class Closure, std::size_t ...I>
        void foreach_element_(Closure&& closure, std::index_sequence<I...>){
            (closure(std::integral_constant<std::size_t, I>{}), ...);
        }
        template<class Closure>
        void foreach_element(Closure &&closure){
            foreach_element_(std::forward<Closure>(closure), std::index_sequence_for<Elements...>{});
        }

    public:
        constexpr heterogeneous_array() noexcept
        {
            foreach_element([&](auto I) {
                using Element = ElementN<I.value>;
                if constexpr (!std::is_trivially_default_constructible_v<Element>) {
                    void *address = &storage[offset_table[I]];
                    new(reinterpret_cast<Element *>(address)) Element();
                }
            });
        }

        // TODO : non-default ctr
        // TODO : move ctr / copy ctr
        heterogeneous_array(const heterogeneous_array&) = delete;
        heterogeneous_array(heterogeneous_array&&) = delete;

        Storage& data(){ return storage; }
        const Storage& data() const { return storage; }

        template<class T>
        constexpr T& get(std::size_t index){
            assert(typeindex_table[index] == typeid(T));

            const std::size_t offset = offset_table[index];
            void* address = &storage[offset];
            return *reinterpret_cast<T*>(address);
        }

        ~heterogeneous_array(){
            foreach_element([&](auto I) {
                using Element = ElementN<I.value>;
                if constexpr (!std::is_trivially_destructible_v<Element>) {
                    get<Element>(I.value).~Element();
                }
            });
        };
    };

}