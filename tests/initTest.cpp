#if defined(_WIN32) || defined(_WIN64)
#include <filesystem>
#else
#include <experimental/filesystem>
#endif
#include "args.h"
#include "conf.h"
#include "init.h"

#include "catch.hpp"

namespace fs = std::experimental::filesystem;

namespace vcbld {

  TEST_CASE("Initialization checks", "[init]") {
    args::New("app");
    CHECK(fs::exists("src"));
    CHECK(fs::exists("src/main.cpp"));
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