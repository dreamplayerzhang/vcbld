#if defined(_WIN32) || defined(_WIN64)
#include <filesystem>
#else
#include <experimental/filesystem>
#endif

#include "prep.h"

#include "catch.hpp"

namespace fs = std::experimental::filesystem;

namespace vcbld {
PrepClass prepClass;
TEST_CASE("Preparation tests", "[prep]") {
  CHECK(prepClass.stripLibName("libname.so") == "name");
  CHECK(prepClass.stripLibName("libvcbld.lib") == "vcbld");
  CHECK(prepClass.sourceFilesSinPath() == "main.cpp ");
  CHECK(
      prepClass.sourceFiles() ==
      "\"" +
          (fs::canonical(prepClass.projPath()) / "src" / "main.cpp").string() +
          "\" ");
  CHECK(prepClass.cmakeOutput() == "");
}
} // namespace vcbld