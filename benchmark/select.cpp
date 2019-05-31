#include "benchmark_utils.hpp"

#include <tower120/ecs/Component.hpp>
#include <tower120/ecs/Entity.hpp>
#include <tower120/ecs/Registry.hpp>

#include <memory>
#include <vector>
#include <iostream>

using namespace tower120::ecs;
using namespace ranges;

struct Position{
    double x,y;
};

struct Color{
    unsigned char r = 0;
    unsigned char g = 0;
    unsigned char b = 0;
};

struct PositionComponent : Component<PositionComponent>{
    double x,y;
};
struct ColorComponent : Component<ColorComponent>{
    unsigned char r = 0;
    unsigned char g = 0;
    unsigned char b = 0;
};

//using ColorComponent = Component<Color>;

using PixelEntity = Entity<PositionComponent, ColorComponent>;

struct PixelClass : Position, Color{
    std::byte p[50];    // make even size
};

int main() {
    const std::size_t times = 1000;
    const std::size_t size  = 100'000;

    std::vector<std::unique_ptr<PixelClass>> classes;
    std::vector<std::unique_ptr<PixelEntity>> entities;

    // 0. sizes
    if (false){
        std::cout << "sizeof(IComponent) "  << sizeof(IComponent) << std::endl;
        std::cout << "sizeof(IEntity) "  << sizeof(IEntity) << std::endl;
        std::cout << "sizeof(Color) "  << sizeof(Color) << std::endl;
        std::cout << "sizeof(ColorComponent) "  << sizeof(ColorComponent) << std::endl;
        std::cout << "sizeof(PositionComponent) "  << sizeof(PositionComponent) << std::endl;

        std::cout << "sizeof(PixelClass) "  << sizeof(PixelClass) << std::endl;
        std::cout << "sizeof(PixelEntity) "  << sizeof(PixelEntity) << std::endl;
    }

    // 1. Fill
    for(std::size_t i = 0; i < size; ++i){
        {
            Color& color = *classes.emplace_back( std::make_unique<PixelClass>() );
            color.r = i;
            color.g = i;
            color.b = i;
        }
        {
            PixelEntity& entity = *entities.emplace_back( std::make_unique<PixelEntity>() );
            ColorComponent& color = entity.get<ColorComponent>();
            color.r = i;
            color.g = i;
            color.b = i;
        }
    }

    // 2. Entity access
    {
        std::cout << "class : "/* << std::endl*/;

        std::size_t sum = 0;
        auto t = benchmark(times, [&](){
          for (auto& ptr : classes){
              Color& color = *ptr;
              sum += color.r;
              sum += color.g;
              sum += color.b;
          }
        });

        std::cout << sum << " "  << t << std::endl;
    }
    {
        std::cout << "entity direct access : "/* << std::endl*/;

        std::size_t sum = 0;
        auto t = benchmark(times, [&](){
          for (auto& ptr : entities){
              ColorComponent& color = ptr->get<ColorComponent>();
              sum += color.r;
              sum += color.g;
              sum += color.b;
          }
        });

        std::cout << sum << " "  << t << std::endl;
    }
    {
        std::cout << "entity type erasured access : "/* << std::endl*/;

        std::size_t sum = 0;
        auto t = benchmark(times, [&](){
          for (auto& ptr : entities){
              IEntity& entity = *ptr;
              ColorComponent& color = entity.get<ColorComponent>();
              sum += color.r;
              sum += color.g;
              sum += color.b;
          }
        });

        std::cout << sum << " "  << t << std::endl;
    }

    // 3. Registry
    {
        Registry registry;
        registry.update(entities | view::indirect);

        std::cout << "registry access : "/* << std::endl*/;

        std::size_t sum = 0;
        auto t = benchmark(times, [&](){
          for (auto [color] : registry.select<ColorComponent>()){
              sum += color.r;
              sum += color.g;
              sum += color.b;
          }
        });

        std::cout << sum << " "  << t << std::endl;
    }

    return 0;
}