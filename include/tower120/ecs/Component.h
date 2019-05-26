#pragma once

#include <vector>
#include "util/monotonic_counter.h"
#include "ComponentBase.h"

namespace tower120::ecs{
    template<class Derived>
    class Component : public ComponentBase {
        friend IEntity;
        template<class...> friend class Entity;

        inline const static ComponentType type_id = util::monotonic_counter<ComponentType, ComponentBase>::get();
        inline static std::vector<detail::entity_offset_t> offset_for_entity;    // index = entity_type_id
    public:
        Component() noexcept
            : ComponentBase(type_id)
        {}

        friend ComponentType type_id(const Component& component){
            return component.type_id;
        }
    };
}