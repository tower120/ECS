#pragma once

#include <vector>
#include "tower120/ecs/detail/common.hpp"

namespace tower120::ecs{
    using ComponentType = unsigned short;

    class IComponent{
        template<class> friend class Component;
        friend ComponentType type_id(const IComponent& component) noexcept;

        const ComponentType type_id;
        explicit IComponent(ComponentType type_id) noexcept : type_id(type_id) {}

        // no need for virtual destructor -
        // heterogeneous_array will call concrete Component destructor
    };

    ComponentType type_id(const IComponent& component) noexcept {
        return component.type_id;
    }

    template<class T>
    const constexpr bool is_component = std::is_base_of_v<IComponent, T>;
}