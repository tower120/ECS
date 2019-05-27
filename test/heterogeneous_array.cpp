#include <tower120/ecs/util/heterogeneous_array.hpp>

#include "test_utils.h"

#include <tuple>

using namespace tower120::ecs::util;

template<class ...Ts>
void fuzzy_test_tuple(){
    using Array = heterogeneous_array<Ts...>;
    using Tuple = std::tuple<Ts...>;

    /*std::cout
        << Array::storage_size << " "
        << sizeof(Tuple) << std::endl;*/
    REQUIRE( Array().data().size() == sizeof(Tuple) );
}

void test_offset_table(){
    using Array = heterogeneous_array<bool, short, void* , short, bool, bool>;

    REQUIRE(Array().data().size() == 24);
    REQUIRE_EQUAL(Array::offset_table, {0, 2, 8, 16, 18, 19});
}

void test_get(){
    using Array = heterogeneous_array<bool, short, std::string>;
    Array array;

    array.get<bool>(0) = true;
    array.get<short>(1) = 12;
    array.get<std::string>(2) = "Hello";

    REQUIRE(array.get<bool>(0) == true);
    REQUIRE(array.get<short>(1) == 12);
    REQUIRE(array.get<std::string>(2) == "Hello");
}

// requires ASAN for testing
void test_misaligned_address(){
    struct Position {
        double x,y;
    };

    struct Test final{
        char payload;
        heterogeneous_array<Position> hp;

        Test(){ (void)payload; }
    };

    Test test;
    REQUIRE(sizeof(test) == sizeof(Position) + alignof(Position));
}

int main(){
    fuzzy_test_tuple<bool>();
    fuzzy_test_tuple<bool, short>();
    fuzzy_test_tuple<bool, void*, short>();
    fuzzy_test_tuple<bool, short, void*>();

    test_offset_table();

    test_get();

    test_misaligned_address();

    return 0;
}