#pragma once

#include <tuple>
#include <array>

#include <range/v3/view/transform.hpp>

#include "util/span.hpp"
#include "tower120/ecs/util/heterogeneous_array.hpp"
#include "tower120/ecs/util/monotonic_counter.hpp"
#include "tower120/ecs/util/type_traits.hpp"

#include "tower120/ecs/detail/common.hpp"
#include "IComponent.hpp"

namespace tower120::ecs{

    using EntityType = unsigned short;

    class IEntity {
    public:
        const EntityType type_id;
    protected:
        using ComponentTypeOffset = std::pair<ComponentType, std::size_t>;
        using ComponentsOffsetTableSpan = nonstd::span<const ComponentTypeOffset>;
    private:
        // switch to offsetof whenever possible
        void* m_components_ptr;
        ComponentsOffsetTableSpan m_components_offset_table;
    protected:
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
    };


    template <class ...Components>
    class Entity final : public IEntity {
        static_assert(sizeof...(Components) > 0, "Must be at least one Component.");
        static_assert(util::is_unique<Components...>, "Components must be unique.");
        static_assert((is_component<Components> && ...), "Components only!");

        template<int I>
        using ComponentN = util::NthType<I, Components...>;

        template<class Closure, std::size_t ...I>
        static void foreach_component_(Closure&& closure, std::index_sequence<I...>){
            (closure(std::integral_constant<std::size_t, I>{}), ...);
        }
        template<class Closure>
        static void foreach_component(Closure &&closure){
            foreach_component_(std::forward<Closure>(closure), std::index_sequence_for<Components...>{});
        }

        util::heterogeneous_array<Components...> m_components;

        using ComponentsOffsetTable = std::array<ComponentTypeOffset, sizeof...(Components) >;
        template<std::size_t ...I>
        static ComponentsOffsetTable make_components_offset_table(std::index_sequence<I...>){
            return
            { std::make_pair(
                ComponentN<I>::type_id,    // non-constxepr!
                decltype(m_components)::offset_table[I]
            )... };
        }
        inline static const ComponentsOffsetTable m_components_offset_table = make_components_offset_table(std::index_sequence_for<Components...>{});

        template<int I>
        static void register_entity() noexcept {
            using Component = ComponentN<I>;
            constexpr const auto offset = decltype(m_components)::offset_table[I];
            static_assert(offset < std::numeric_limits<detail::entity_offset_t>::max(), "Entity size too big. Increase entity_offset_t.");

            Component::offset_for_entity().resize(type_id+1, -1);
            Component::offset_for_entity()[type_id] = offset;
        }

        static void register_entities() noexcept {
            foreach_component([&](auto I){ register_entity<I.value>(); });
        }

        static inline const bool registered = [](){
            register_entities();
            return true;
        }();
    public:
        inline const static EntityType type_id = util::monotonic_counter<EntityType, IEntity>::get();

        template<class Component>
        Component& get() noexcept {
            static_assert(util::has_type<Component, Components...>, "Component does not exists in Entity.");
            constexpr const int N = util::type_index<Component, Components...>;
            return m_components.template get<Component>(N);
        }

        Entity() noexcept
            : IEntity(type_id, &m_components, m_components_offset_table)
        {
            // register before use
            (void)registered;
        }

        // TODO : update EntityBase::m_components on move / copy
        Entity(const Entity&) = delete;
        Entity(Entity&&) = delete;
    };

}