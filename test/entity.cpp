#include <tower120/ecs/Entity.h>
#include <tower120/ecs/Component.h>

#include "test_utils.h"

using namespace tower120::ecs;

// TODO: Get rid of Component?

struct Other : Component<Other>{};

struct Position : Component<Position>{
    double x,y;
};

struct Color : Component<Color>{
    unsigned char r,g,b;
};

using PixelEntity = Entity<Position, Color>;

template<class E>
void test_get(E& entity){
    entity.template get<Position>().x = 1;
    entity.template get<Position>().y = 2;
    entity.template get<Color>().r = 10;
    entity.template get<Color>().g = 20;
    entity.template get<Color>().b = 30;

    REQUIRE(entity.template get<Position>().x == 1);
    REQUIRE(entity.template get<Position>().y == 2);
    REQUIRE(entity.template get<Color>().r == 10);
    REQUIRE(entity.template get<Color>().g == 20);
    REQUIRE(entity.template get<Color>().b == 30);
}

void test_Entity_get(){
    PixelEntity pixel;
    IEntity& entity = pixel;

    REQUIRE(!entity.get_if<Other>());
    test_get(entity);
    test_get(pixel);
}

int main(){
    test_Entity_get();
    return 0;
}