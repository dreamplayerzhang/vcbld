#include <cstdlib>
#include <iostream>

#include "args.h"
#include "help.h"

#define MAJOR_VERSION 0
#define MINOR_VERSION 1
#define PATCH_VERSION 0

using namespace vcbld;

int main(int argc, char *argv[]) {
  if (argc >= 2) {
    if (strcmp(argv[1], "new") == 0) {
      if (!argv[2]) {
        Args::New("app");
      } else {
        Args::New(static_cast<std::string>(argv[2]));
      }
    } else if (strcmp(argv[1], "configure") == 0) {
      Args::configure();
    } else if (strcmp(argv[1], "restore") == 0) {
      Args::restore();
    } else if (strcmp(argv[1], "generate") == 0) {
      Args::generate();
    } else if (strcmp(argv[1], "build") == 0) {
      if (!argv[2]) {
        try {
          Args::build("debug");
        } catch (const std::exception &e) {
          std::cout << "Build configuration or entry not available!"
                    << std::endl;
        }
      } else {
        if (strcmp(argv[2], "release") == 0) {
          try {
            Args::build("release");
          } catch (const std::exception &e) {
            std::cout << "Build configuration or entry not available!"
                      << std::endl;
          }
        } else if (strcmp(argv[2], "debug") == 0) {
          try {
            Args::build("debug");
          } catch (const std::exception &e) {
            std::cout << "Build configuration or entry not available!"
                      << std::endl;
          }
        }
      }
    } else if (strcmp(argv[1], "clean") == 0) {
      Args::clean();
    } else if (strcmp(argv[1], "available") == 0) {
      Args::available();
    } else if (strcmp(argv[1], "run") == 0) {
      if (!argv[2]) {
        try {
          Args::run("debug");
        } catch (const std::exception &e) {
          std::cout << "Build configuration or entry not available!"
                    << std::endl;
        }
      } else {
        if (strcmp(argv[2], "release") == 0) {
          try {
            Args::run("release");
          } catch (const std::exception &e) {
            std::cout << "Build configuration or entry not available!"
                      << std::endl;
          }
        } else if (strcmp(argv[2], "debug") == 0) {
          try {
            Args::run("debug");
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
        Args::search(static_cast<std::string>(argv[2]));
      }
    } else if (strcmp(argv[1], "add") == 0) {
      if (!argv[2]) {
        std::cout << "Please enter a package name to add it." << std::endl;
      } else {
        Args::add(static_cast<std::string>(argv[2]));
      }
    } else if (strcmp(argv[1], "remove") == 0) {
      if (!argv[2]) {
        std::cout << "Please enter a package name to remove it." << std::endl;
      } else {
        Args::remove(static_cast<std::string>(argv[2]));
      }
    } else if (strcmp(argv[1], "list") == 0) {
      Args::list();
    } else if (strcmp(argv[1], "help") == 0) {
      std::cout << "\nvcbld command line tools, version: \t" << MAJOR_VERSION
                << "." << MINOR_VERSION << "." << PATCH_VERSION << std::endl;
      Help::mainHelp();
    } else if (strcmp(argv[1], "vcpkg") == 0) {
      if (!argv[2]) {
        std::string help = "help";
        Args::vcpkg(help);
      } else {
        std::string args;
        for (int i = 2; i < argc; i++) {
          args += " ";
          args += argv[i];
        }
        Args::vcpkg(args);
      }
    } else if (strcmp(argv[1], "--version") == 0) {
      std::cout << MAJOR_VERSION << "." << MINOR_VERSION << "." << PATCH_VERSION
                << std::endl;
    } else {
      std::cout << "Unknown command!" << std::endl;
      std::cout << "Type Help to get the help menu." << std::endl;
      std::exit(1);
    }
  } else {
    std::cout << "Unknown command!" << std::endl;
    std::cout << "Type Help to get the help menu." << std::endl;
    std::exit(1);
  }
}
