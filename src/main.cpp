#include <boost/filesystem.hpp>
#include <cstdlib>
#include <iostream>

#include "args.h"
#include "help.h"
#include "init.h"

#define MAJOR_VERSION 0
#define MINOR_VERSION 1
#define PATCH_VERSION 0

namespace fs = boost::filesystem;

using namespace vcbld;

int main(int argc, char *argv[]) {
  fs::path vcbldPath = argv[0];
  vcbldPath = vcbldPath.parent_path();

  if (argc >= 2) {
    if (strcmp(argv[1], "new") == 0) {
      if (!argv[2]) {
        args::New("app");
      } else {
        args::New(static_cast<std::string>(argv[2]));
      }
    } else if (strcmp(argv[1], "setup") == 0) {
      init::setup(vcbldPath);
    } else if (strcmp(argv[1], "configure") == 0) {
      args::configure(vcbldPath);
    } else if (strcmp(argv[1], "restore") == 0) {
      args::restore(vcbldPath);
    } else if (strcmp(argv[1], "gen") == 0) {
      args::generate(vcbldPath);
    } else if (strcmp(argv[1], "build") == 0) {
      if (!argv[2]) {
        try {
          args::build("debug", vcbldPath);
        } catch (...) {
          std::cout << "Build configuration or entry not available!"
                    << std::endl;
        }
      } else {
        if (strcmp(argv[2], "release") == 0) {
          try {
            args::build("release", vcbldPath);
          } catch (...) {
            std::cout << "Build configuration or entry not available!"
                      << std::endl;
          }
        } else if (strcmp(argv[2], "debug") == 0) {
          try {
            args::build("debug", vcbldPath);
          } catch (...) {
            std::cout << "Build configuration or entry not available!"
                      << std::endl;
          }
        }
      }
    } else if (strcmp(argv[1], "clean") == 0) {
      args::clean(vcbldPath);
    } else if (strcmp(argv[1], "available") == 0) {
      args::available(vcbldPath);
    } else if (strcmp(argv[1], "run") == 0) {
      if (!argv[2]) {
        try {
          args::run("debug", vcbldPath);
        } catch (...) {
          std::cout << "Build configuration or entry not available!"
                    << std::endl;
        }
      } else {
        if (strcmp(argv[2], "release") == 0) {
          try {
            args::run("release", vcbldPath);
          } catch (...) {
            std::cout << "Build configuration or entry not available!"
                      << std::endl;
          }
        } else if (strcmp(argv[2], "debug") == 0) {
          try {
            args::run("debug", vcbldPath);
          } catch (const std::exception &e) {
            std::cout << "Build configuration or entry not available!"
                      << std::endl;
          }
        }
      }
    } else if (strcmp(argv[1], "search") == 0) {
      if (!argv[2]) {
        std::cout << "Please enter a package name to search for." << std::endl;
      } else {
        args::search(static_cast<std::string>(argv[2]), vcbldPath);
      }
    } else if (strcmp(argv[1], "add") == 0) {
      if (!argv[2]) {
        std::cout << "Please enter a package name to add it." << std::endl;
      } else {
        args::add(static_cast<std::string>(argv[2]), vcbldPath);
      }
    } else if (strcmp(argv[1], "remove") == 0) {
      if (!argv[2]) {
        std::cout << "Please enter a package name to remove it." << std::endl;
      } else {
        args::remove(static_cast<std::string>(argv[2]), vcbldPath);
      }
    } else if (strcmp(argv[1], "list") == 0) {
      args::list(vcbldPath);
    } else if (strcmp(argv[1], "help") == 0) {
      std::cout << "\nvcbld command line tools, version: \t" << MAJOR_VERSION
                << "." << MINOR_VERSION << "." << PATCH_VERSION << std::endl;
      help::mainHelp();
    } else if (strcmp(argv[1], "vcpkg") == 0) {
      if (!argv[2]) {
        std::string help = "help";
        args::vcpkg(help, vcbldPath);
      } else {
        std::string vcpkgArgs;
        for (int i = 2; i < argc; i++) {
          vcpkgArgs += " ";
          vcpkgArgs += argv[i];
        }
        args::vcpkg(vcpkgArgs, vcbldPath);
      }
    } else if (strcmp(argv[1], "cmake") == 0) {
      if (!argv[2]) {
        std::string empty = " ";
        args::cmake(empty, vcbldPath);
      } else {
        std::string cmakeArgs;
        for (int i = 2; i < argc; i++) {
          cmakeArgs += " ";
          cmakeArgs += argv[i];
        }
        args::cmake(cmakeArgs, vcbldPath);
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
