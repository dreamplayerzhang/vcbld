#include "help.h"
#include <iomanip>
#include <iostream>

namespace vcbld::help
{

void mainHelp()
{
  const char *lit = R"lit(
Usage: vcbld [sdk-command]

SDK commands:
    setup                           Creates a conf.json in the vcbld directory.
    new     [app|dylib|statlib]     Starts a new project.
    restore                         Restore missing dependencies in a project.
    build   [debug|release]         Builds the project.
    clean                           Clean build output(s).
    run     [debug|release]         Executes the project.
    includes                        Generates includePath.json file.
    gen                             Generates CMakeLists.txt file.
    list                            Shows a list of packages added to the project.
    available                       Shows a list of installed vcpkg packages.
    search                          Search within installed vcpkg packages.
    add                             Add package to the project.
    remove                          Remove package from the project.
    vcpkg   [vcpkg command]         Runs vcpkg commands.
    cmake   [cmake arguments]       Runs cmake in the output directory generating makefiles.
    make                            Runs make in the output directory.
    help                            Show help.

    --version                       Current vcbld version in use.

    )lit";

  std::cout << std::setw(4) << lit << std::endl;
}
} // namespace vcbld::help
