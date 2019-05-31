```cpp
/*type*/ ComponentType;

class IComponent{};

ComponentType type_id(const IComponent&);

template<class Component>
const constexpr bool is_component;
```

```cpp
template<class Derived>
class Component{};

template<class Component>
ComponentType type_id(const Component&);

template<class Component>
ComponentType type_id();
```

Usage:

```cpp
struct Color : Component<Color>{
    unsigned char r,g,b;
};
```
Single inheritance only!

----

Move to ?
```cpp
struct Color{
    unsigned char r,g,b;
};
using ColorComponent = Component<Color>;

Entity<Color> entity;
Color& color = entity.get<Color>;
```