#pragma once

#include <vector>
#include "detail/common.h"

namespace tower120::ecs{

    using ComponentType = unsigned short;

    class ComponentBase{
    private:
        const ComponentType type_id;

    protected:
        explicit ComponentBase(ComponentType type_id) : type_id(type_id) {}

    public:
        friend ComponentType type_id(const ComponentBase& component){
            return component.type_id;
        }
    };

    template<class Component>
    const constexpr bool is_component = std::is_base_of_v<ComponentBase, Component>;

    namespace detail{
        using EnityOffsets = std::vector<detail::entity_offset_t>;

        //using ComponentOffsets = std::vector<EnityOffsets*>;
        //inline static ComponentOffsets component_offsets;
    }

}