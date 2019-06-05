#include <tower120/ecs/Entity.hpp>
#include <tower120/ecs/Component.hpp>
#include <tower120/ecs/IndexedContainer.hpp>

#include "test_utils.h"

using namespace tower120::ecs;
using namespace ranges;

struct Other : Component<Other>{};

struct Position : Component<Position>{
    double x;
    double y;
};

struct Color : Component<Color>{
    unsigned char r;
    unsigned char g;
    unsigned char b;
};

struct Index : Component<Index>{
    int index;
};

template<class T>
const auto transform_get = view::transform([](auto& entity) -> decltype(auto) {
    return std::get<T&>(entity);
});


int main(){
    auto check_equal = [](auto& entities, auto&&...values){
        auto selection = entities.template select<Position,Color,Index>().collect();
        REQUIRE_EQUAL(
            selection | transform_get<Index> | view::transform(&Index::index),
            {values...}
        );
    };

    IndexedContainer entities{};

    // Test emplace
    // 1. first
    {
        auto& entity = **entities.emplace(entities.end(), std::make_unique<Entity<Position,Color,Index>>());
        entity.get<Index>().index = 0;
        check_equal(entities, 0);
    }
    // 2. at the end
    {
        auto& entity = **entities.emplace(entities.end(), std::make_unique<Entity<Position,Color,Index>>());
        entity.get<Index>().index = 1;
        check_equal(entities, 0, 1);
    }
    // 3. at the begin
    {
        auto& entity = **entities.emplace(entities.begin(), std::make_unique<Entity<Position,Color,Index>>());
        entity.get<Index>().index = -1;
        check_equal(entities, -1, 0, 1);
    }
    // 4. in the middle
    {
        auto& entity = **entities.emplace(entities.begin()+1, std::make_unique<Entity<Position,Color,Index>>());
        entity.get<Index>().index = 100;
        check_equal(entities, -1, 100, 0, 1);
    }

    // Test erase
    // 1. very last
    {
        entities.erase(entities.begin()+3);
        check_equal(entities, -1, 100, 0);
    }
    // 2. in the middle
    {
        entities.erase(entities.begin()+1);
        check_equal(entities, -1, 0);
    }
    // 3. very first
    {
        entities.erase(entities.begin());
        check_equal(entities, 0);
    }

    return 0;
}