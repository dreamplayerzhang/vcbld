#if defined(_WIN32) || defined(_WIN64)
#include <filesystem>
#else
#include <experimental/filesystem>
#endif

#include "args.h"
#include "builder.h"

#include "catch.hpp"

namespace vcbld {

Builder builder("debug");

TEST_CASE("Build app", "[build]") {
  SECTION("Build")
  builder.build();
  CHECK(fs::exists(builder.projPath() + "/bin/debug/main.o"));

  SECTION("run")
  args::run("debug");
}

TEST_CASE("Clean output", "[clean]") {
  args::clean("debug");
  CHECK(!fs::exists("bin/debug/main.o"));
  CHECK(!fs::exists("bin/debug/tests"));
}
} // namespace vcbld