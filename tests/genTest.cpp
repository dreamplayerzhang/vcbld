#if defined(_WIN32) || defined(_WIN64)
#include <filesystem>
#else
#include <experimental/filesystem>
#endif

#include "args.h"
#include "conf.h"

#include "catch.hpp"

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