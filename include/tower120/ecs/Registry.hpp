#pragma once

#include <unordered_map>
#include <vector>

#include <range/v3/algorithm/min_element.hpp>
#include <range/v3/algorithm/max_element.hpp>
#include <range/v3/view/indirect.hpp>
#include <range/v3/view/map.hpp>

#include "util/type_traits.hpp"
#include "util/binary_search.hpp"
#include "tower120/ecs/util/apply_range.hpp"

#include "IComponent.hpp"
#include "IEntity.hpp"

namespace tower120::ecs{
    class Registry {
        // TODO: object pool, reuse ComponentList
        using ComponentList  = std::vector<std::pair<std::size_t, IComponent*>>;
        using ComponentTypes = std::unordered_map<ComponentType, ComponentList>;
        ComponentTypes component_types;
    public:
        template<class Rng>
        void update(Rng&& rng){
            component_types.clear();
            auto process_component = [&](std::size_t index, ComponentType component_type_id, IComponent& component){
                ComponentList& components = component_types[component_type_id];
                components.emplace_back(index, &component);
            };
            std::size_t index = 0;
            for(IEntity& entity : rng){
                for(auto&&[component_type_id, component] : entity.get_all()){
                    process_component(index, component_type_id, component);
                }
                index++;
            }
        }

        template<class ...Components, class Closure, class ZipIterator, std::size_t ...Is>
        void call_closure(Closure&& closure, ZipIterator& zip_iterator, std::index_sequence<Is...>){
            closure(
                *static_cast< util::TypeN<Is, Components...>* >(zip_iterator[Is]->second)
                ...
            );
        }

        // TODO: optionally return IEntity
        // TODO: switch from Closure to range
        template<class ...Components, class Closure>
        void select(Closure&& closure) /*-> ForwardRange< std::tuple<Component&> > unknown size */ {
            using namespace util;
            using namespace ranges;
            static_assert((is_component<Components> && ...));
            static_assert(is_unique<Components...>);

            // TODO : replace with tuple<ComponentList*...> . Compiler optimize tuples better
            std::array<ComponentList*, sizeof...(Components)> components;
            std::array<ComponentList::iterator, sizeof...(Components)> zip_iterator;

            // 0. find and store Components&
            {
                bool all_components_found = true;
                static_for<sizeof...(Components)>([&](auto n){
                    using Component = TypeN<n.value, Components...>;
                    auto found = component_types.find( type_id<Component>() );
                    if (found == component_types.end()){
                        all_components_found = false;
                        return;
                    }
                    components[n.value] = &found->second;
                });
                if (!all_components_found) return;
            }

            // 2. Fill zip_iterator
            static_for<sizeof...(Components)>([&](auto n){
                zip_iterator[n.value] = components[n.value]->begin();
            });

            while(true){
                // 3. Align zip_iterator
                {
                    bool all_components_aligned = true;
                    auto zip_indices = zip_iterator | view::indirect | view::keys;
                    const std::size_t max_index = *ranges::max_element(zip_indices);
                    auto get_first = [](auto&& pair) -> decltype(auto) { return pair.first; };
                    static_for<sizeof...(Components)>([&](auto n){
                        ComponentList::iterator begin = zip_iterator[n.value];
                        if (begin->first == max_index) return;  // fast skip check
                        ComponentList::iterator end   = components[n.value]->end();

                        std::optional<ComponentList::iterator> found = util::binary_search(begin, end, max_index, less{}, get_first);
                        if (!found){
                            all_components_aligned = false;
                            return;
                        }
                        zip_iterator[n.value] = *found;
                    });
                    if (!all_components_aligned) return;
                }

                // 4. call closure
                {
                    call_closure<Components...>(closure, zip_iterator, std::make_index_sequence<sizeof...(Components)>{});
                }

                // 5. Increase iterator
                {
                    bool reach_end = false;
                    static_for<sizeof...(Components)>([&](auto n){
                        zip_iterator[n.value]++;
                        if (zip_iterator[n.value] == components[n.value]->end()) {
                            reach_end = true;
                        }
                    });
                    if (reach_end) return;
                }
            }
        }
    };

}