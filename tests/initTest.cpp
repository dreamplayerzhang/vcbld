#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "pch.h"

using namespace vcbld;

TEST_CASE("Testing Catch. Fail") { REQUIRE(1 == 2); }
TEST_CASE("Testing Catch. Pass") {
  REQUIRE(1 == 1);
  args::New("app");
  Init init(fs::current_path().parent_path());
  init.init("app");
  CHECK(!init.cCompilers().empty());
  CHECK(!init.cppCompilers().empty());
  CHECK(!init.cmakePaths().empty());
  CHECK(!init.makePaths().empty());
  CHECK(!init.archiverPaths().empty());
  init.setCompiler(1);
  init.setCppCompiler(1);
  init.setCmake(1);
  init.setMake(1);
  init.setArchiver(1);
  init.setVcpkg(1);
  init.setup();
  std::cout << "\nvcbld command line tools, version: \t" << vcbld_MAJOR_VERSION
            << "." << vcbld_MINOR_VERSION << "." << vcbld_PATCH_VERSION
            << std::endl;
  args::help();
}
