#pragma once

#include <vector>
#include "tower120/ecs/util/monotonic_counter.hpp"
#include "IComponent.hpp"

namespace tower120::ecs{
    class IEntity;

    // Derived must have single inheritance!
    template<class Derived>
    class Component : public IComponent {
        friend IEntity;
        template<class...> friend class Entity;
        template<class T, std::enable_if_t<is_component<T>, int>>
        friend ComponentType type_id() noexcept;
        template<class T, std::enable_if_t<is_component<T>, int>>
        friend ComponentType type_id(const T&) noexcept;

        static ComponentType type_id() noexcept {
            const static ComponentType type_id = util::monotonic_counter<ComponentType, IComponent>::get();
            return type_id;
        }

        // Prevent static order initialization fiasco
        using offset_for_entity_t = std::vector<detail::entity_offset_t>;
        static offset_for_entity_t& offset_for_entity() noexcept {
            static offset_for_entity_t offsets;
            return offsets;
        }
    public:
        Component() noexcept
            : IComponent(type_id())
        {}
    };

    template<class T, std::enable_if_t<is_component<T>, int> = 0>
    ComponentType type_id(const T&) noexcept {
        return T::type_id();
    }
    template<class T, std::enable_if_t<is_component<T>, int> = 0>
    ComponentType type_id() noexcept {
        return T::type_id();
    }
}