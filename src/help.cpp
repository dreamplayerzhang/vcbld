#include "help.h"

namespace vcbld {
namespace help {
void mainHelp() {
  const char *lit = R"lit(
Usage: vcbld [sdk-command]

SDK commands:
    setup                           Creates a conf.json in the project directory.
    new        [app|dylib|statlib]  Starts a new project.
    restore                         Restore missing dependencies in a project.
    build      [debug|release]      Builds the project.
    clean      [debug|release]      Clean build output(s).
    run        [debug|release]      Executes the project.
    includes                        Generates includePath.json file.
    gen                             Generates CMakeLists.txt file.
    list                            Shows a list of packages added to the project.
    all                             Shows a list of installed vcpkg packages.
    find       [package name]       Search within installed vcpkg packages.
    search     [package name]       Search vcpkg ports for packages to install.
    add        [package names]      Add package to the project.
    remove     [package names]      Remove package from the project.
    install    [package names]      Install packages using vcpkg and add project.
    uninstall  [package names]      Uninstall packages using vcpkg and remove from project.
    vcpkg      [vcpkg command]      Runs vcpkg commands.
    cmake      [config + args]      Runs cmake in the output directory using the vcpkg toolchain.
    make       [make arguments]     Runs make in the output directory.
    help                            Show help.

    --version                         Current vcbld version in use.

    )lit";

  std::cout << std::setw(4) << lit << std::endl;
}
} // namespace help
} // namespace vcbld
