#pragma once

#include <vector>
#include "tower120/ecs/detail/common.hpp"

namespace tower120::ecs{
    using ComponentType = unsigned short;

    class IComponent{
        template<class> friend class Component;
        friend ComponentType type_id(const IComponent& component);

        const ComponentType type_id;
        explicit IComponent(ComponentType type_id) : type_id(type_id) {}
    };

    ComponentType type_id(const IComponent& component){
        return component.type_id;
    }

    template<class Component>
    const constexpr bool is_component = std::is_base_of_v<IComponent, Component>;
}