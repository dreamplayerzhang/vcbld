#include <cstdlib>
#include <cstring>
#include <errno.h>
#if defined(_WIN32)
#include <filesystem>
#else
#include <experimental/filesystem>
#endif
#include <iostream>
#include <vector>

#include "args.h"
#include "help.h"
#include "init.h"

#define MAJOR_VERSION 0
#define MINOR_VERSION 1
#define PATCH_VERSION 0

namespace fs = std::experimental::filesystem;

using namespace vcbld;

std::string findVcbld(const std::string &PATH);

int main(int argc, char *argv[]) {
  fs::path vcbldPath;
  fs::path temp = argv[0];
  temp = temp.parent_path();

  if (temp == "") {
    try {
      std::string vcbldExec = std::getenv("PATH");
      vcbldPath = fs::canonical(findVcbld(vcbldExec));
    } catch (const std::exception &e) {
      std::cout << "Error starting vcbld." << std::endl;
      std::cerr << e.what() << " " << errno << std::endl;
    }
  } else {
    vcbldPath = fs::canonical(temp);
  }

  if (!fs::exists(vcbldPath.string() + "/vcpkg") && !fs::exists(vcbldPath.string() + "/vcpkg.exe")) {
    std::cout << "The vcpkg executable was not found!\nPlease add vcbld to the "
                 "same directory as your vcpkg executable!"
              << std::endl;
    std::exit(1);
  }

  if (argc >= 2) {
    if (strcmp(argv[1], "new") == 0) {
      if (argc < 3) {
        args::New("app");
        init::setup(vcbldPath);
      } else {
        args::New(static_cast<std::string>(argv[2]));
        init::setup(vcbldPath);
      }
    } else if (strcmp(argv[1], "setup") == 0) {
      init::setup(vcbldPath);
    } else if (strcmp(argv[1], "restore") == 0) {
      args::restore();
    } else if (strcmp(argv[1], "includes") == 0) {
      args::includes();
    } else if (strcmp(argv[1], "gen") == 0) {
      args::generate();
    } else if (strcmp(argv[1], "commands") == 0) {
      args::commands();
    } else if (strcmp(argv[1], "build") == 0) {
      if (argc < 3) {
        args::build("debug");
      } else {
        if (strcmp(argv[2], "release") == 0) {
          args::build("release");
        } else if (strcmp(argv[2], "debug") == 0) {
          args::build("debug");
        }
      }
    } else if (strcmp(argv[1], "clean") == 0) {
      args::clean();
    } else if (strcmp(argv[1], "available") == 0) {
      args::available();
    } else if (strcmp(argv[1], "run") == 0) {
      if (argc < 3) {
        try {
          args::run("debug");
        } catch (...) {
          std::cout << "Build configuration or entry not available!"
                    << std::endl;
        }
      } else {
        if (strcmp(argv[2], "release") == 0) {
          try {
            args::run("release");
          } catch (...) {
            std::cout << "Build configuration or entry not available!"
                      << std::endl;
          }
        } else if (strcmp(argv[2], "debug") == 0) {
          try {
            args::run("debug");
          } catch (const std::exception &e) {
            std::cout << "Build configuration or entry not available!"
                      << std::endl;
            std::cerr << e.what() << std::endl;
          }
        }
      }
    } else if (strcmp(argv[1], "search") == 0) {
      if (argc < 3) {
        std::cout << "Please enter a package name to search for." << std::endl;
      } else {
        args::search(static_cast<std::string>(argv[2]));
      }
    } else if (strcmp(argv[1], "add") == 0) {
      if (argc < 3) {
        std::cout << "Please enter a package name to add it." << std::endl;
      } else {
        std::vector<std::string> packages;
        for (int i = 2; i < argc; i++) {
          packages.push_back(static_cast<std::string>(argv[i]));
        }
        args::add(packages);
      }
    } else if (strcmp(argv[1], "remove") == 0) {
      if (argc < 3) {
        std::cout << "Please enter a package name to remove it." << std::endl;
      } else {
        std::vector<std::string> packages;
        for (int i = 2; i < argc; i++) {
          packages.push_back(static_cast<std::string>(argv[i]));
        }
        args::remove(packages);
      }
    } else if (strcmp(argv[1], "list") == 0) {
      args::list();
    } else if (strcmp(argv[1], "help") == 0) {
      std::cout << "\nvcbld command line tools, version: \t" << MAJOR_VERSION
                << "." << MINOR_VERSION << "." << PATCH_VERSION << std::endl;
      help::mainHelp();
    } else if (strcmp(argv[1], "install") == 0) {
      if (argc < 3) {
        std::cout << "Please enter package names to install." << std::endl;
      } else {
        std::string package;
        std::vector<std::string> packages;
        for (int i = 2; i < argc; i++) {
          package += " ";
          package += argv[i];
          packages.push_back(static_cast<std::string>(argv[i]));
        }
        args::install(package);
        args::add(packages);
      }
    } else if (strcmp(argv[1], "uninstall") == 0) {
      if (argc < 3) {
        std::cout << "Please enter package names to uninstall." << std::endl;
      } else {
        std::string package;
        std::vector<std::string> packages;
        for (int i = 2; i < argc; i++) {
          package += " ";
          package += argv[i];
          packages.push_back(static_cast<std::string>(argv[i]));
        }
        args::remove(packages);
        args::uninstall(package);
      }
    } else if (strcmp(argv[1], "vcpkg") == 0) {
      if (argc < 3) {
        args::vcpkg("help");
      } else {
        std::string vcpkgArgs;
        for (int i = 2; i < argc; i++) {
          vcpkgArgs += " ";
          vcpkgArgs += argv[i];
        }
        args::vcpkg(vcpkgArgs);
      }
    } else if (strcmp(argv[1], "cmake") == 0) {
      if (argc < 3) {
        std::string empty = " ";
        args::cmake(empty);
      } else {
        std::string cmakeArgs;
        for (int i = 2; i < argc; i++) {
          cmakeArgs += " ";
          cmakeArgs += argv[i];
        }
        args::cmake(cmakeArgs);
      }
    } else if (strcmp(argv[1], "make") == 0) {
      if (argc < 3) {
        std::string empty = " ";
        args::make(empty);
      } else {
        std::string makeArgs;
        for (int i = 2; i < argc; i++) {
          makeArgs += " ";
          makeArgs += argv[i];
        }
        args::make(makeArgs);
      }
    } else if (strcmp(argv[1], "--version") == 0) {
      std::cout << MAJOR_VERSION << "." << MINOR_VERSION << "." << PATCH_VERSION
                << std::endl;
    } else {
      std::cout << "Unknown command!" << std::endl;
      std::cout << "Type help to get the help menu." << std::endl;
      std::exit(1);
    }
  } else {
    std::cout << "Unknown command!" << std::endl;
    std::cout << "Type help to get the help menu." << std::endl;
    std::exit(1);
  }
}

std::string findVcbld(const std::string &PATH) {
  std::string temp;
  size_t foundVcpkg = PATH.find("/vcpkg/");
  if (PATH.find("/vcpkg/") != std::string::npos) {
    temp = PATH.substr(0, foundVcpkg + 6);
  }
  size_t foundSep = temp.find_last_of(":");
  temp = temp.substr(foundSep + 1, temp.length());
  if (temp[0] == '~') {
    std::string home = std::getenv("HOME");
    temp.replace(0, 1, home);
  }

  return temp;
}
