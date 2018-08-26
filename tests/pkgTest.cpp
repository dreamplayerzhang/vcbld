#if defined(_WIN32) || defined(_WIN64)
#include <filesystem>
#else
#include <experimental/filesystem>
#endif
#include <vector>

#include "args.h"
#include "pkg.h"

#include "catch.hpp"

namespace vcbld {
PkgClass pkgClass;
TEST_CASE("package tests", "[pkg]") {

  CHECK(fs::exists("package.json"));
  std::vector<std::string> v{"pack"};
  args::restore();
  args::available();
  args::add(v);
  args::remove(v);
  args::list();
  args::find("pack");
  args::search("pack");
  args::vcpkg("help");
}
} // namespace vcbld
