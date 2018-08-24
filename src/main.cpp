#include <cstdlib>
#include <cstring>
#include <errno.h>
#if defined(_WIN32) || defined(_WIN64)
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

std::string parseQuotes(const char* arg);

int main(int argc, char *argv[]) {
  fs::path vcbldPath = argv[0];
  vcbldPath = vcbldPath.parent_path();

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
        if (strcmp(argv[2], "release") == 0 || strcmp(argv[2], "-r") == 0) {
          args::build("release");
        } else if (strcmp(argv[2], "debug") == 0 ||
                   strcmp(argv[2], "-g") == 0) {
          args::build("debug");
        }
      }
    } else if (strcmp(argv[1], "clean") == 0) {
      if (argc < 3) {
        args::clean("debug");
      } else {
        if (strcmp(argv[2], "release") == 0 || strcmp(argv[2], "-r") == 0) {
          args::clean("release");
        } else if (strcmp(argv[2], "debug") == 0 ||
                   strcmp(argv[2], "-g") == 0) {
          args::clean("debug");
        }
      }
    } else if (strcmp(argv[1], "all") == 0) {
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
        if (strcmp(argv[2], "release") == 0 || strcmp(argv[2], "-r") == 0) {
          try {
            args::run("release");
          } catch (...) {
            std::cout << "Build configuration or entry not available!"
                      << std::endl;
          }
        } else if (strcmp(argv[2], "debug") == 0 ||
                   strcmp(argv[2], "-g") == 0) {
          try {
            args::run("debug");
          } catch (const std::exception &e) {
            std::cout << "Build configuration or entry not available!"
                      << std::endl;
            std::cerr << e.what() << std::endl;
          }
        }
      }
    } else if (strcmp(argv[1], "find") == 0) {
      if (argc < 3) {
        std::cout << "Please enter a package name to search for." << std::endl;
      } else {
        args::find(static_cast<std::string>(argv[2]));
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
    } else if (strcmp(argv[1], "search") == 0) {
      if (argc < 3) {
        std::cout << "Please enter a package names to search for." << std::endl;
      } else {
        args::search(static_cast<std::string>(argv[2]));
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
      std::string cmakeArgs = "";
      if (argc < 3) {
        args::cmake(cmakeArgs);
      } else {
        if (strcmp(argv[2], "debug") == 0 || strcmp(argv[2], "-g") == 0) {
            cmakeArgs += "-DCMAKE_BUILD_TYPE=Debug";
          } else if (strcmp(argv[2], "release") == 0 || strcmp(argv[2], "-r") == 0) {
            cmakeArgs += "-DCMAKE_BUILD_TYPE=Release";
          } else {
            cmakeArgs += parseQuotes(argv[2]);
          }
        if (argc >= 4)
        for (int i = 3; i < argc; i++) {
          cmakeArgs += " ";
          cmakeArgs += parseQuotes(argv[i]);
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

std::string parseQuotes(const char* arg) {
  std::string temp = static_cast<std::string>(arg);
  if (temp.find(" ") != std::string::npos) {
    return "\"" + temp + "\"";
  } else {
    return temp;
  }
}