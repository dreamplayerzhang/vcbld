#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "pch.h"

#include "args.h"
#include "init.h"
#include "vcbld.h"

namespace fs = std::experimental::filesystem;

using namespace vcbld;

TEST_CASE("Testing Catch. Fail") { REQUIRE(1 == 2); }
TEST_CASE("Testing Catch. Pass") {
  REQUIRE(1 == 1);
    args::New("app");
    Init init(fs::current_path().parent_path());
    init.init("app");
    CHECK(init.cCompilers().size() > 0);
    CHECK(init.cppCompilers().size() > 0);
    CHECK(init.cmakePaths().size() > 0);
    CHECK(init.makePaths().size() > 0);
    CHECK(init.archiverPaths().size() > 0);
    init.setCompiler(1);
    init.setCppCompiler(1);
    init.setCmake(1);
    init.setMake(1);
    init.setArchiver(1);
    init.setVcpkg(1);
    init.setup();
  std::cout << "\nvcbld command line tools, version: \t" << MAJOR_VERSION << "."
            << MINOR_VERSION << "." << PATCH_VERSION << std::endl;
  args::help();
}
