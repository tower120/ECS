```cpp
class IEntity {
public:
    const EntityType type_id;
    
    // very fast, one indirection
    template<class Container>
    Container& get() noexcept;
    
    // fast, two indirections    
    IContainer& get(ContainerType) noexcept;    
}
```

```cpp
template<class ...Containers>
class Entity final : IEntity {
public:
    // virtually no-op
    template<class Container>
    Container& get() noexcept;
}
```