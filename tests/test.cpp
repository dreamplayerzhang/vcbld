#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include "args.h"
#include "vcbld.h"

using namespace vcbld;

TEST_CASE("Testing Catch. Fail") { REQUIRE(1 == 2); }
TEST_CASE("Testing Catch. Pass") {
  REQUIRE(1 == 1);
  std::cout << "\nvcbld command line tools, version: \t" << MAJOR_VERSION << "."
            << MINOR_VERSION << "." << PATCH_VERSION << std::endl;
  args::help();
}
