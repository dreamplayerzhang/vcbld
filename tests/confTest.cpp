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
  CHECK(confClass.projPath() != "");
  CHECK(confClass.cCompilerPath() != "");
  CHECK(confClass.cppCompilerPath() != "");
  CHECK(confClass.vcpkgDirPath() != "");
  CHECK(confClass.cmakePath() != "");
  CHECK(confClass.makePath() != "");
  CHECK(confClass.archiverPath() != "");
  CHECK(confClass.architecture() != "");
  CHECK(confClass.architecture() != "");
  CHECK(confClass.projectName() != "");
  CHECK(confClass.language() == "c++");
  CHECK(confClass.standard() != "");
  CHECK(confClass.binaryName() != "");
  CHECK(confClass.binaryType() == "app");
  CHECK(confClass.outputDirectory() != "");
  CHECK(confClass.sourceDirectory() != "");
}
} // namespace vcbld
