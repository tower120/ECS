#pragma once

#include <vector>
#include "util/monotonic_counter.h"
#include "ComponentFwd.h"

namespace tower120::ecs{
    class ComponentBase{
        const ComponenType type_id;

    protected:
        explicit ComponentBase(ComponenType type_id) : type_id(type_id) {}

    public:
        friend ComponenType type_id(const ComponentBase& component){
            return component.type_id;
        }
    };

    template<class Derived>
    class Component : public ComponentBase {
        friend EntityBase;
        inline const static ComponenType type_id = util::monotonic_counter<ComponenType, ComponentBase>::get();

        inline static std::vector<unsigned short> offset_for_entity;    // index = entity_type_id

    public:
        Component() noexcept
            : ComponentBase(type_id)
        {}

        friend ComponenType type_id(const Component& component){
            return component.type_id;
        }
    };
}