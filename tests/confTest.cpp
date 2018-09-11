#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "pch.h"

namespace vcbld {

TEST_CASE("Initialization checks", "[init]") {
  CHECK(fs::exists("src"));
  CHECK(fs::exists("src/main.cpp"));
  CHECK(fs::exists("conf.json"));
  CHECK(fs::exists("vcbld.json"));
  CHECK(fs::exists("package.json"));
}

TEST_CASE("Conf.json reading test", "[conf]") {
  ConfClass confClass;
  CHECK(!confClass.projPath().empty());
  CHECK(!confClass.cCompilerPath().empty());
  CHECK(!confClass.cppCompilerPath().empty());
  CHECK(!confClass.vcpkgDirPath().empty());
  CHECK(!confClass.cmakePath().empty());
  CHECK(!confClass.makePath().empty());
  CHECK(!confClass.archiverPath().empty());
  CHECK(!confClass.architecture().empty());
  CHECK(!confClass.architecture().empty());
  CHECK(!confClass.projectName().empty());
  CHECK(confClass.language() == "c++");
  CHECK(!confClass.standard().empty());
  CHECK(!confClass.binaryName().empty());
  CHECK(confClass.binaryType() == "app");
  CHECK(!confClass.outputDirectory().empty());
  CHECK(!confClass.sourceDirectory().empty());
}
} // namespace vcbld
