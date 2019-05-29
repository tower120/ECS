#pragma once

#include <vector>
#include "tower120/ecs/util/monotonic_counter.hpp"
#include "IComponent.hpp"

namespace tower120::ecs{
    class IEntity;

    template<class Derived>
    class Component : public IComponent {
        friend IEntity;
        template<class...> friend class Entity;
        template<class, class> friend ComponentType type_id();

        static ComponentType type_id(){
            const static ComponentType type_id = util::monotonic_counter<ComponentType, IComponent>::get();
            return type_id;
        }

        // Prevent static order initialization fiasco
        using offset_for_entity_t = std::vector<detail::entity_offset_t>;
        static offset_for_entity_t& offset_for_entity(){
            static offset_for_entity_t offsets;
            return offsets;
        }
    public:
        Component() noexcept
            : IComponent(type_id())
        {}

        friend ComponentType type_id(const Component&){
            return Component::type_id();
        }
    };

    template<class T, typename = std::enable_if_t< is_component<T> >>
    ComponentType type_id(){
        return T::type_id();
    }
}