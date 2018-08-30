#include "catch.hpp"
#include "pch.h"

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