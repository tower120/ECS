#pragma once

#include "util/monotonic_counter.hpp"
#include "util/type_traits.hpp"
#include "util/span.hpp"

#include "IComponent.hpp"

namespace tower120::ecs{

    using EntityType = unsigned short;

    class IEntity {
        template <class...> friend class Entity;
    public:
        const EntityType type_id;
    private:
        using ComponentTypeOffset = std::pair<ComponentType, std::size_t>;
        using ComponentsOffsetTableSpan = nonstd::span<const ComponentTypeOffset>;

        // switch to offsetof whenever possible
        void* m_components_ptr;
        ComponentsOffsetTableSpan m_components_offset_table;

        IEntity(
            EntityType type_id,
            void* m_components_ptr,
            ComponentsOffsetTableSpan m_components_offset_table
        ) noexcept
            : type_id(type_id)
            , m_components_ptr(m_components_ptr)
            , m_components_offset_table(m_components_offset_table)
        {}

        // TODO : update EntityBase::m_components on move / copy
        IEntity(const IEntity&) = delete;
        IEntity(IEntity&&) = delete;

    public:
        template<class Component>
        Component* get_if() noexcept {
            static_assert(is_component<Component>, "Components only!");

            if (Component::offset_for_entity().size() <= type_id) return nullptr;
            const auto offset = Component::offset_for_entity()[type_id];
            if (offset < 0) return nullptr;

            void* address = static_cast<std::byte*>(m_components_ptr) + offset;
            return reinterpret_cast<Component*>(address);
        }

        template<class Component>
        Component& get() noexcept {
            static_assert(is_component<Component>, "Components only!");

            const auto offset = Component::offset_for_entity()[type_id];
            assert(offset >= 0);

            void* address = static_cast<std::byte*>(m_components_ptr) + offset;
            return *reinterpret_cast<Component*>(address);
        }

        auto get_all() noexcept /* -> Range< pair<component_type_id, IComponent&> > */{
            return m_components_offset_table | ranges::view::transform(
                [&](const ComponentTypeOffset& in) -> std::pair<ComponentType, IComponent&>{
                    void* address = static_cast<std::byte*>(m_components_ptr) + in.second;
                    return {in.first, *static_cast<IComponent*>(address)};
                });
        }

        virtual ~IEntity() = default;
    };

}