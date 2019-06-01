#include <tower120/ecs/Entity.hpp>
#include <tower120/ecs/Component.hpp>
#include <tower120/ecs/Registry.hpp>

#include "test_utils.h"

#include <vector>
#include <memory>

#include <range/v3/view/indirect.hpp>
#include <range/v3/action/push_back.hpp>
#include <range/v3/algorithm/copy.hpp>
#include <range/v3/iterator/insert_iterators.hpp>

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

struct Depth : Component<Depth>{
    double z = 300;
};

int main(){
    //        1  2  3  4
    // Pos    *  *     *
    // Color  *     *  *
    // Depth  *     *  *

    std::vector<std::unique_ptr<IEntity>> entities;
    entities.emplace_back(
        std::make_unique<Entity<Position, Color, Depth>>()
    );
    entities.emplace_back(
        std::make_unique<Entity<Position>>()
    );
    entities.emplace_back(
        std::make_unique<Entity<Color, Depth>>()
    );
    entities.emplace_back(
        std::make_unique<Entity<Position, Color, Depth>>()
    );

    Registry registry;
    registry.update(entities | view::indirect);

    // 1 component
    {
        std::vector selection = registry.select<IEntity, Position>().collect();
        REQUIRE(selection.size() == 3);
        REQUIRE(&std::get<IEntity&>(selection[0]) == entities[0].get());
        REQUIRE(&std::get<IEntity&>(selection[1]) == entities[1].get());
        REQUIRE(&std::get<IEntity&>(selection[2]) == entities[3].get());
    }
    {
        std::vector selection = registry.select<IEntity, Color>().collect();
        REQUIRE(selection.size() == 3);
        REQUIRE(&std::get<IEntity&>(selection[0]) == entities[0].get());
        REQUIRE(&std::get<IEntity&>(selection[1]) == entities[2].get());
        REQUIRE(&std::get<IEntity&>(selection[2]) == entities[3].get());
    }
    {
        std::vector selection = registry.select<IEntity, Depth>().collect();
        REQUIRE(selection.size() == 3);
        REQUIRE(&std::get<IEntity&>(selection[0]) == entities[0].get());
        REQUIRE(&std::get<IEntity&>(selection[1]) == entities[2].get());
        REQUIRE(&std::get<IEntity&>(selection[2]) == entities[3].get());
    }
    // 2 components
    {
        std::vector selection = registry.select<IEntity, Position, Color>().collect();
        REQUIRE(selection.size() == 2);
        REQUIRE(&std::get<IEntity&>(selection[0]) == entities.front().get());
        REQUIRE(&std::get<IEntity&>(selection[1]) == entities.back().get());
    }
    {
        std::vector selection = registry.select<IEntity, Color, Depth>().collect();
        REQUIRE(selection.size() == 3);
        REQUIRE(&std::get<IEntity&>(selection[0]) == entities[0].get());
        REQUIRE(&std::get<IEntity&>(selection[1]) == entities[2].get());
        REQUIRE(&std::get<IEntity&>(selection[2]) == entities[3].get());
    }
    {
        std::vector selection = registry.select<IEntity, Position, Depth>().collect();
        REQUIRE(selection.size() == 2);
        REQUIRE(&std::get<IEntity&>(selection[0]) == entities.front().get());
        REQUIRE(&std::get<IEntity&>(selection[1]) == entities.back().get());
    }
    // 3 components
    {
        std::vector selection = registry.select<IEntity, Position, Color, Depth>().collect();
        REQUIRE(selection.size() == 2);
        REQUIRE(&std::get<IEntity&>(selection[0]) == entities.front().get());
        REQUIRE(&std::get<IEntity&>(selection[1]) == entities.back().get());
    }

    return 0;
}