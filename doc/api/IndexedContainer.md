```cpp
template<
    class Container = std::vector<std::unique_ptr<IEntity>>,
    class GetEntity = IndirectFunctor>
class IndexedContainer{
public:
    auto begin();
    auto end();
    
    decltype(auto) erase(const iterator&)
    template<class ...Args>
    decltype(auto) emplace(const iterator& before, Args&&... args)
    
    template<class ...Components>
    decltype(auto) select()    
}
```