#pragma once

#include "IEntity.hpp"
#include "Registry.hpp"

#include <range/v3/view/transform.hpp>
#include <range/v3/algorithm/find.hpp>

#include <memory>

namespace tower120::ecs{

    template<class EntityContainer>
    class IndexedContainerChain{

    };

    struct IndirectFunctor{
        template<class T>
        decltype(auto) operator()(T&& value) const {
            return *value;
        }
    };

    template<
        class Container = std::vector<std::unique_ptr<IEntity>>,
        class GetEntity = IndirectFunctor>
    class IndexedContainer {
        Container container;
        Registry registry;

        using iterator = typename Container::iterator;
        using value_type = typename Container::value_type;
    public:
        auto begin() {
            return container.begin();
        }
        auto end() {
            return container.end();
        }

        // TODO: postpone registry update

        decltype(auto) erase(const iterator& iter){
            const std::size_t index = ranges::distance(container.begin(), iter);
            registry.erase(index);
            return container.erase(iter);
        }

        /*void erase(IEntity& entity){
            auto found = ranges::find(container, entity, GetEntity{});
            assert(found != container.end());
            container.erase(found);

            // TODO : update registry
            // TODO : move out element as return
        }*/

        template<class ...Args>
        decltype(auto) emplace(const iterator& before, Args&&... args){
            const std::size_t index = ranges::distance(container.begin(), before);
            auto iter = container.emplace(before, std::forward<Args>(args)...);
            auto& entity = GetEntity{}(*iter);
            registry.emplace(index, entity);
            return iter;
        }

        /*template<class ...Args>
        decltype(auto) emplace_back(){

        }*/

        template<class ...Components>
        decltype(auto) select(){
            return registry.select<Components...>();
        }
    };

}