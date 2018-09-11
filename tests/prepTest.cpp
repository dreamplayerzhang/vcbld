#include "catch.hpp"
#include "pch.h"

namespace vcbld {
PrepClass prepClass;
TEST_CASE("Preparation tests", "[prep]") {
  CHECK(prepClass.stripLibName("libname.so") == "name");
  CHECK(prepClass.stripLibName("libvcbldlib.lib") == "vcbldlib");
  CHECK(prepClass.stripLibName("vcbldlib.lib") == "vcbldlib");
  CHECK(prepClass.sourceFilesSinPath() == "main.cpp ");
  std::string path = (fs::canonical(prepClass.projPath()) / "src" / "main.cpp").string();
  prepClass.posixify(path);
  CHECK(
      prepClass.sourceFiles() ==
      "\"" +
          path +
          "\" ");
  CHECK(prepClass.cmakeOutput().empty());
}
} // namespace vcbld
