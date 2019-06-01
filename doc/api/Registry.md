```cpp
class Registry{
public:
    template<class T, class ...Components> // T = IEntity/Component
    RegistrySelectRange<Components> select();
}

template<class ...Components>
class RegistrySelectRange /* ForwardRange */{
public:
    template<template<class...> class Container = std::vector>
    auto collect();
}
```