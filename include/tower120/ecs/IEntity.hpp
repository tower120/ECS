#pragma once

#include "util/monotonic_counter.hpp"
#include "util/type_traits.hpp"
#include "util/span.hpp"

#include "IComponent.hpp"

namespace tower120::ecs{

    using EntityType = unsigned short;

    class IEntity {
        template <class...> friend class Entity;
        friend EntityType type_id(const IEntity&) noexcept;

        const EntityType type_id;

        using ComponentTypeOffset = std::pair<ComponentType, std::size_t>;
        using ComponentsOffsetTableSpan = nonstd::span<const ComponentTypeOffset>;

        static std::vector<ComponentsOffsetTableSpan>& m_components_offset_table() noexcept {
            static std::vector<ComponentsOffsetTableSpan> m_components_offset_table_;
            return m_components_offset_table_;
        }

        // switch to offsetof whenever possible
        void* m_components_ptr;     // set directly from Entity

        explicit IEntity(EntityType type_id) noexcept
            : type_id(type_id)
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
            return static_cast<Component*>(address);
        }

        template<class Component>
        Component& get() noexcept {
            static_assert(is_component<Component>, "Components only!");

            const auto offset = Component::offset_for_entity()[type_id];
            assert(offset >= 0);

            void* address = static_cast<std::byte*>(m_components_ptr) + offset;
            return *static_cast<Component*>(address);
        }

        auto get_all() noexcept /* -> Range< pair<component_type_id, IComponent&> > */{
            return m_components_offset_table()[type_id] | ranges::view::transform(
                [&](const ComponentTypeOffset& in) -> std::pair<ComponentType, IComponent&>{
                    void* address = static_cast<std::byte*>(m_components_ptr) + in.second;
                    assert(in.first == tower120::ecs::type_id(*static_cast<IComponent*>(address)));
                    return {in.first, *static_cast<IComponent*>(address)};
                });
        }

        virtual ~IEntity() = default;
    };

    EntityType type_id(const IEntity& entity) noexcept {
        return entity.type_id;
    }

    template<class T>
    const constexpr bool is_entity = std::is_base_of_v<IEntity, T>;
}