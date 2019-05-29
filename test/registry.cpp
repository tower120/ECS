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
    double x,y;
};

struct Color : Component<Color>{
    unsigned char r = 0;
    unsigned char g = 0;
    unsigned char b = 0;
};

using PixelEntity = Entity<Position, Color>;

int main(){
    std::vector<std::unique_ptr<IEntity>> entities;
    entities.emplace_back(
        std::make_unique<PixelEntity>()
    );
    entities.emplace_back(
        std::make_unique<PixelEntity>()
    );

    Registry registry;
    registry.update(entities | view::indirect);

    // TODO : `select` accepts tuple too
    auto range = registry.select<Color>();
    for(auto [color] : range){
        static_assert(std::is_same_v<decltype(color), Color&>);
        std::cout
            << (int)color.r
            << (int)color.g
            << (int)color.b
        << std::endl;
    }

    return 0;
}