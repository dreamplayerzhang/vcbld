#include "catch.hpp"
#include "pch.h"


namespace vcbld {

ConfClass confClass;
TEST_CASE("Generate includePath.json", "[includes]") {
  args::includes();
  CHECK(fs::exists(confClass.projPath() + "/includePath.json"));
}

TEST_CASE("Generate CMake files", "[gen]") {
  args::generate();
  CHECK(fs::exists(confClass.projPath() + "/CMakeLists.txt"));
  CHECK(fs::exists(confClass.projPath() + "/src/CMakeLists.txt"));
}
} // namespace vcbld