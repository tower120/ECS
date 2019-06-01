#pragma once

#include <unordered_map>
#include <vector>

#include <range/v3/algorithm/min_element.hpp>
#include <range/v3/algorithm/max_element.hpp>
#include <range/v3/view/indirect.hpp>
#include <range/v3/view/map.hpp>
#include <range/v3/view/facade.hpp>

#include "util/type_traits.hpp"
#include "util/binary_search.hpp"
#include "tower120/ecs/util/apply_range.hpp"

#include "IComponent.hpp"
#include "IEntity.hpp"

namespace tower120::ecs{

    template<bool, class ...>
    class RegistrySelectRange;

    // EntitiesIndex ?
    class Registry {
        template<bool, class ...> friend class RegistrySelectRange;

        // TODO: object pool, reuse ComponentList?
        using ComponentList  = std::vector<std::pair<std::size_t /*IEntity index*/, IComponent*>>;    // could be std::map<iterator, IComponent*>, for O(log(N)) mutations
        using ComponentTypes = std::unordered_map<ComponentType, ComponentList>;
        ComponentTypes component_types;

        std::vector<IEntity*> entities;
    public:
        // TODO : change to operator=, or constructor ?
        template<class Rng>
        void update(Rng&& rng){
            component_types.clear();
            entities.clear();
            if constexpr (ranges::range_cardinality<Rng>::value == ranges::cardinality::finite){
                entities.reserve(rng.size());
            }

            auto process_component = [&](std::size_t index, ComponentType component_type_id, IComponent& component){
                ComponentList& components = component_types[component_type_id];
                components.emplace_back(index, &component);
            };

            std::size_t index = 0;
            for(IEntity& entity : rng){
                for(auto&&[component_type_id, component] : entity.get_all()){
                    assert(component_type_id == type_id(component));
                    process_component(index, component_type_id, component);
                }
                entities.emplace_back(&entity);
                index++;
            }
        }

        // make sense only for EntitiesStableContainer<vector<unique_ptr>>
        // With EntitiesList, EntitiesStableVector:
        //
        // void entity_inserted(after index, IEntity&)
        // void entity_erased(at index)
        // void entity_updated(at index)
        // void update(rng, from index = 0)     ??

        template<class T, class ...Components>
        auto select() noexcept {
            if constexpr (std::is_same_v<T, IEntity>){
                return RegistrySelectRange<true, Components...>{*this};
            } else {
                return RegistrySelectRange<false, T, Components...>{*this};
            }
        }

        // TODO : select<IEntity, Components>
        // TODO : select<IEntity, Components>()
        //  .keys<Key1, Key2, Key3>( Key1{} == 2, Key2{} != 3 )
        //  .key_equal<Key1>(5)
        //  .key_greater<Key1>(5)
        //  .key_lesser<Key1>(5)
        //  .key_greater_or_equal<Key1>(5)
        //  .key_lesser_or_equal<Key1>(5)
    };


    template<bool ReturnEntities, class ...Components>
    class RegistrySelectRange
        : public ranges::view_facade<RegistrySelectRange<ReturnEntities, Components...>>
    {
        static_assert(sizeof...(Components) > 0);
        static_assert(util::all_unique<Components...>);

        friend ranges::range_access;

        using ComponentList = Registry::ComponentList;

        // TODO: replace with ZipSentinel?
        using ComponentLists = util::tuple_array<ComponentList*, sizeof...(Components)>;
        using ZipIterator    = util::tuple_array<ComponentList::iterator, sizeof...(Components)>;

        ComponentLists component_lists;
        ZipIterator zip_iterator;

        std::vector<IEntity*>* entities;

        bool the_end = false;

        void increase_zip_iterator(){
            util::static_for<sizeof...(Components)>([&](auto n){
                ++std::get<n.value>(zip_iterator);
                if (std::get<n.value>(zip_iterator) == std::get<n.value>(component_lists)->end()) {
                    the_end = true;
                }
            });
        }

        void align_zip_iterator() {
            using namespace ranges;
            using namespace util;

            while (true){
                if (the_end) return;

                bool all_components_aligned = true;
                {
                    bool all_same = true;
                    const std::size_t max_index =
                        tuple_array_accumulate(zip_iterator, std::get<0>(zip_iterator)->first,
                            [&](std::size_t max, ComponentList::iterator iterator){
                                const std::size_t index = iterator->first;
                                if (max != index) {
                                    all_same = false;
                                    return std::max(max, index);
                                }
                                return max;
                            });
                    if (all_same) return;

                    static_for<sizeof...(Components)>([&](auto n){
                        if (!all_components_aligned) return;
                        ComponentList::iterator begin = std::get<n.value>(zip_iterator);
                        ComponentList::iterator end   = std::get<n.value>(component_lists)->end();

                        std::optional<ComponentList::iterator> found =
                            util::binary_search(
                                begin, end, max_index, less{},
                                [](auto&& pair) -> decltype(auto) { return pair.first; }
                            );
                        if (!found){
                            all_components_aligned = false;
                            return;
                        }
                        std::get<n.value>(zip_iterator) = std::move(*found);
                    });
                }
                if (all_components_aligned) return;

                increase_zip_iterator();
            }
        }

        // -------------------------------------------------------------------
        //          Cursor
        // -------------------------------------------------------------------
        auto read() const {
            if constexpr(ReturnEntities) {
                const auto index = std::get<0>(zip_iterator)->first;
                IEntity& entity = *(*entities)[index];
                return std::tuple<IEntity&, Components&...>{
                    entity,
                    *static_cast<Components*>(
                        std::get< util::type_index<Components, Components...> >(zip_iterator)->second
                    )...
                };
            } else {
                return std::tuple<Components&...>{
                    *static_cast<Components*>(
                        std::get< util::type_index<Components, Components...> >(zip_iterator)->second
                    )...
                };
            }
        }

        bool equal(ranges::default_sentinel_t) const {
            return the_end;
        }

        void init(Registry& registry){
            using namespace util;

            // 1. find and store ComponentList*'s
            {
                bool all_components_found = true;
                static_for<sizeof...(Components)>([&](auto n){
                    using Component = TypeN<n.value, Components...>;
                    auto found = registry.component_types.find( type_id<Component>() );
                    if (found == registry.component_types.end()){
                        all_components_found = false;
                        return;
                    }
                    std::get<n.value>(component_lists) = &found->second;
                });
                if (!all_components_found) {
                    the_end = true;
                    return;
                }
            }

            // 2. Fill zip_iterator
            {
                static_for<sizeof...(Components)>([&](auto n){
                    std::get<n.value>(zip_iterator) = std::get<n.value>(component_lists)->begin();
                });
            }

            align_zip_iterator();
        }

        void next() {
           increase_zip_iterator();
           align_zip_iterator();
        }

    public:
        RegistrySelectRange() = default;
        explicit RegistrySelectRange(Registry& registry) noexcept
            : entities(&registry.entities)
        {
            init(registry);
        }

        template<template<class...> class Container = std::vector>
        auto collect(){
            return Container<decltype(read())>{*this};
        }
    };

}