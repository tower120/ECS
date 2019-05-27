```cpp
class IEntity {
public:
    const EntityType type_id;
    
    // very fast, one indirection
    // return nullptr if no such Component
    template<class Component>
    Component* get_if() noexcept;
    
    // very fast, one indirection
    // no Component existence checks
    template<class Component>
    Component& get() noexcept;    
    
    // ??
    // fast, two indirections    
    // IComponent* get(ComponentType) noexcept;
    
    // virtually no-op
    // Range< pair<component_type_id, IComponent&> >
    auto get_all() noexcept;
}
```

```cpp
template<class ...Containers>
class Entity final : IEntity {
public:
    static const EntityType type_id;
    
    // virtually no-op
    // Component existence compile time check   
    template<class Component>
    Component& get() noexcept;
}
```