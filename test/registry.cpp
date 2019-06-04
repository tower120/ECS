#include <tower120/ecs/Entity.hpp>
#include <tower120/ecs/Component.hpp>
#include <tower120/ecs/Registry.hpp>

#include "test_utils.h"

#include <vector>
#include <memory>

#include <range/v3/view/indirect.hpp>
#include <range/v3/view/addressof.hpp>
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

void test_update(){
    //        1  2  3  4  5  6
    // Pos    *  *     *
    // Color  *     *  *     *
    // Depth  *     *  *  *  *

    std::vector<std::unique_ptr<IEntity>> entities;
    // 1
    entities.emplace_back(
        std::make_unique<Entity<Position, Color, Depth>>()
    );
    // 2
    entities.emplace_back(
        std::make_unique<Entity<Position>>()
    );
    // 3
    entities.emplace_back(
        std::make_unique<Entity<Color, Depth>>()
    );
    // 4
    entities.emplace_back(
        std::make_unique<Entity<Position, Color, Depth>>()
    );
    // 5
    entities.emplace_back(
        std::make_unique<Entity<Depth>>()
    );
    // 6
    entities.emplace_back(
        std::make_unique<Entity<Color, Depth>>()
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
        REQUIRE(selection.size() == 4);
        REQUIRE(&std::get<IEntity&>(selection[0]) == entities[0].get());
        REQUIRE(&std::get<IEntity&>(selection[1]) == entities[2].get());
        REQUIRE(&std::get<IEntity&>(selection[2]) == entities[3].get());
        REQUIRE(&std::get<IEntity&>(selection[3]) == entities[5].get());
    }
    {
        std::vector selection = registry.select<IEntity, Depth>().collect();
        REQUIRE(selection.size() == 5);
        REQUIRE(&std::get<IEntity&>(selection[0]) == entities[0].get());
        REQUIRE(&std::get<IEntity&>(selection[1]) == entities[2].get());
        REQUIRE(&std::get<IEntity&>(selection[2]) == entities[3].get());
        REQUIRE(&std::get<IEntity&>(selection[2]) == entities[3].get());
    }
    // 2 components
    {
        std::vector selection = registry.select<IEntity, Position, Color>().collect();
        REQUIRE(selection.size() == 2);
        REQUIRE(&std::get<IEntity&>(selection[0]) == entities[0].get());
        REQUIRE(&std::get<IEntity&>(selection[1]) == entities[3].get());
    }
    {
        std::vector selection = registry.select<IEntity, Color, Depth>().collect();
        REQUIRE(selection.size() == 4);
        REQUIRE(&std::get<IEntity&>(selection[0]) == entities[0].get());
        REQUIRE(&std::get<IEntity&>(selection[1]) == entities[2].get());
        REQUIRE(&std::get<IEntity&>(selection[2]) == entities[3].get());
        REQUIRE(&std::get<IEntity&>(selection[3]) == entities[5].get());
    }
    {
        std::vector selection = registry.select<IEntity, Position, Depth>().collect();
        REQUIRE(selection.size() == 2);
        REQUIRE(&std::get<IEntity&>(selection[0]) == entities[0].get());
        REQUIRE(&std::get<IEntity&>(selection[1]) == entities[3].get());
    }
    // 3 components
    {
        std::vector selection = registry.select<IEntity, Position, Color, Depth>().collect();
        REQUIRE(selection.size() == 2);
        REQUIRE(&std::get<IEntity&>(selection[0]) == entities[0].get());
        REQUIRE(&std::get<IEntity&>(selection[1]) == entities[3].get());
    }
}

void test_emplace_erase(){
    std::vector<std::unique_ptr<IEntity>> entities;
    Registry registry;

    auto get_IEntity = [](auto& tuple) -> IEntity& {
        return std::get<IEntity&>(tuple);
    };

    auto check_equal = [&](){
        std::vector selection = registry.select<IEntity, Position, Color, Depth>().collect();
        REQUIRE_EQUAL(
            selection | view::transform(get_IEntity) | view::addressof,
            entities | view::indirect | view::addressof);
    };

    // 1 (at the end)
    {
        entities.emplace_back(
            std::make_unique<Entity<Position, Color, Depth>>()
        );
        registry.emplace(0, *entities[0]);
        check_equal();
    }

    // 2 (at the end)
    {
        entities.emplace_back(
            std::make_unique<Entity<Position, Color, Depth>>()
        );
        registry.emplace(1, *entities[1]);
        check_equal();
    }

    // 3 (at the begin)
    {
        entities.emplace(entities.begin(),
            std::make_unique<Entity<Position, Color, Depth>>()
        );
        registry.emplace(0, *entities[0]);
        check_equal();
    }

    // 4 (in the middle)
    {
        entities.emplace(entities.begin()+1,
            std::make_unique<Entity<Position, Color, Depth>>()
        );
        registry.emplace(1, *entities[1]);
        check_equal();
    }


    // Erase
    // 1 (at the end)
    {
        registry.erase(3);
        entities.erase(entities.begin()+3);
        check_equal();
    }

    // 2 (in the middle)
    {
        registry.erase(1);
        entities.erase(entities.begin()+1);
        check_equal();
    }

    // 3 (at the begin)
    {
        registry.erase(0);
        entities.erase(entities.begin()+0);
        check_equal();
    }

    // Insert (in the middle)
    {
        entities.emplace(entities.begin()+1,
            std::make_unique<Entity<Position, Color, Depth>>()
        );
        registry.emplace(1, *entities[1]);
        check_equal();
    }
}

int main(){
    test_update();
    test_emplace_erase();
    return 0;
}