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