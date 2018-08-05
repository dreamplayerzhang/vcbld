#include "help.h"
#include <iomanip>
#include <iostream>

namespace vcbld::Help {

void mainHelp() {
  const char *lit = R"lit(
Usage: vcbld [sdk-command]

SDK commands:
    new     [app|dylib|statlib]     Starts a new project.
    configure                       Configure a vcbld project. 
    restore                         Restore missing dependencies in a project.
    build   [debug|release]         Builds the project.
    clean                           Clean build output(s).
    run     [debug|release]         Executes the project.
    generate                        Generates includePath.txt file.
    list                            Shows a list of packages added to the project.
    available                       Shows a list of all available and installed vcpkg packages.
    search                          Search within installed vcpkg packages.
    add                             Add package to the project.
    remove                          Remove package from the project.
    vcpkg   [vcpkg command]         Runs vcpkg commands. Type vcbld vcpkg help for more info.
    help                            Show help.

    --version                       Current vcbld version in use.

    )lit";

  std::cout << std::setw(4) << lit << std::endl;
}
} // namespace vcbld::Help