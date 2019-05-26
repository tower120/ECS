#pragma once

#include <tuple>
#include <array>

#include "util/heterogeneous_array.h"
#include "util/monotonic_counter.h"

//#include "Component.h"

namespace tower120::ecs{

    using EntityType = unsigned short;


    namespace detail::Entity {
        template <typename...>
        inline constexpr auto is_unique = std::true_type{};

        template <typename T, typename... Rest>
        inline constexpr auto is_unique<T, Rest...> = std::bool_constant<
            (!std::is_same_v<T, Rest> && ...) && is_unique<Rest...>
        >{};
    }



    class EntityBase {
    private:
        void* m_components;
    public:
        const EntityType type_id;

    protected:
        using offset_t = unsigned short;
        explicit EntityBase(EntityType type_id)
            : type_id(type_id)
        {}

    public:
        template<class Component>
        Component& component() noexcept {
             const offset_t offset = Component::offset_for_entity[type_id];
             void* address = (std::byte*)m_components + offset;
             return *reinterpret_cast<Component*>(address);
        }

        //ComponentBase* (*get_component)(EntityBase* entity, ComponenType type);
    };


    template <class ...Components>
    class Entity final : public EntityBase {
        static_assert(detail::Entity::is_unique<Components...>, "Components must be unique for Entity");

         util::heterogeneous_array<Components...> m_components;

    public:
        inline const static EntityType type_id = util::monotonic_counter<EntityType, EntityBase>::get();

        // TODO: register entity

        /**
         *
         * offsetof(EntityBase, m_components)
         *
         */

        Entity() noexcept
            : EntityBase(type_id)
        {
            std::ptrdiff_t base_offset = this - &m_components;
        }

        // TODO : update EntityBase::m_components on move / copy

    };


    // components[component_type_id][entity_type_id] = offset

}