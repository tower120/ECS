#include <tower120/ecs/Entity.hpp>
#include <tower120/ecs/Component.hpp>
#include <tower120/ecs/Registry.hpp>

#include "test_utils.h"

#include <vector>
#include <memory>

#include <range/v3/view/indirect.hpp>

using namespace tower120::ecs;
using namespace ranges;

struct Other : Component<Other>{};

struct Position : Component<Position>{
    double x = 200;
    double y = 100;
};

struct Color : Component<Color>{
    unsigned char r = 10;
    unsigned char g = 20;
    unsigned char b = 30;
};

using PixelEntity = Entity<Position, Color>;

int main(){
    std::vector<std::unique_ptr<IEntity>> entities;
    entities.emplace_back(
        std::make_unique<PixelEntity>()
    );
    /*entities.emplace_back(
        std::make_unique<PixelEntity>()
    );*/

    Registry registry;
    registry.update(entities | view::indirect);

    // TODO : `select` accepts tuple too
    for(auto [color] : registry.select<Color>()){
        static_assert(std::is_same_v<decltype(color), Color&>);
        IComponent& c = color;
        REQUIRE(type_id(c) == type_id<Color>());
        
        std::cout
            << (int)color.r
            << (int)color.g
            << (int)color.b
        << std::endl;
    }

    return 0;
}