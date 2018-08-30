#include "catch.hpp"
#include "pch.h"

namespace vcbld {
PkgClass pkgClass;
TEST_CASE("package tests", "[pkg]") {

  CHECK(fs::exists("package.json"));
  std::vector<std::string> v{"pack"};
  args::restore();
//   args::available();
  args::add(v);
  args::remove(v);
  args::list();
  args::find("pack");
  args::search("pack");
//   args::vcpkg("help");
}
} // namespace vcbld
