#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "initTest.h"

using namespace vcbld;

TEST_CASE("Testing Catch. Fail") {
    REQUIRE(1 == 2);
}
TEST_CASE("Testing Catch. Pass") {
    REQUIRE(1 == 1);
}
