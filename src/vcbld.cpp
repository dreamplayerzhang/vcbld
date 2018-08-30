#include "pch.h"


using namespace vcbld;

std::string parseQuotes(const char *arg);
void setup(Init &init);

int main(int argc, char *argv[]) {
  fs::path vcbldPath = argv[0];
  vcbldPath = vcbldPath.parent_path();

  if (argc >= 2) {
    if (strcmp(argv[1], "new") == 0) {
      Init init(vcbldPath);
      if (argc < 3) {
        args::New("app");
        init.init("app");
      } else {
        args::New(static_cast<std::string>(argv[2]));
        init.init(static_cast<std::string>(argv[2]));
      }
      setup(init);
    } else if (strcmp(argv[1], "setup") == 0) {
      Init init(vcbldPath);
      setup(init);
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
      args::help();
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
        } else if (strcmp(argv[2], "release") == 0 ||
                   strcmp(argv[2], "-r") == 0) {
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

void setup(Init &init) {
  if (fs::exists("conf.json")) {
    std::cout << "conf.json was found in the directory, would you like to "
                 "reconfigure it?\nType yes to reconfigure..."
              << std::endl;
    std::string entry;
    std::cin >> entry;
    if (entry == "yes") {
      fs::remove("conf.json");
      setup(init);
    }
  } else {
    if (init.cCompilers().size() == 0) {
      std::cout << "vcbld couldn't locate a C compiler." << std::endl;
      init.setCompiler();
    } else if (init.cCompilers().size() == 1) {
      init.setCompiler(1);
    } else {
      init.lister(init.cCompilers());
      std::cout << "Please choose a C compiler." << std::endl;
      try {
        int i;
        std::cin >> i;
        init.setCompiler(i);
      } catch (...) {
        init.setCompiler(1);
      }
    }
    if (init.cppCompilers().size() == 0) {
      std::cout << "vcbld couldn't locate a C++ compiler." << std::endl;
      init.setCppCompiler();
    } else if (init.cppCompilers().size() == 1) {
      init.setCppCompiler(1);
    } else {
      init.lister(init.cppCompilers());
      std::cout << "Please choose a C++ compiler." << std::endl;
      try {
        int i;
        std::cin >> i;
        init.setCppCompiler(i);
      } catch (...) {
        init.setCppCompiler(1);
      }
    }
    if (init.cmakePaths().size() == 0) {
      std::cout << "vcbld couldn't locate a cmake executable." << std::endl;
      init.setCmake();
    } else if (init.cmakePaths().size() == 1) {
      init.setCmake(1);
    } else {
      init.lister(init.cmakePaths());
      std::cout << "Please choose a cmake executable." << std::endl;
      try {
        int i;
        std::cin >> i;
        init.setCmake(i);
      } catch (...) {
        init.setCmake(1);
      }
    }
    if (init.makePaths().size() == 0) {
      std::cout << "vcbld couldn't locate a make executable." << std::endl;
      init.setMake();
    } else if (init.makePaths().size() == 1) {
      init.setMake(1);
    } else {
      init.lister(init.makePaths());
      std::cout << "Please choose a make executable." << std::endl;
      try {
        int i;
        std::cin >> i;
        init.setMake(i);
      } catch (...) {
        init.setMake(1);
      }
    }
    if (init.archiverPaths().size() == 0) {
      std::cout << "vcbld couldn't locate an archiver." << std::endl;
      init.setArchiver();
    } else if (init.archiverPaths().size() == 1) {
      init.setArchiver(1);
    } else {
      init.lister(init.archiverPaths());
      std::cout << "Please choose an archiver." << std::endl;
      try {
        int i;
        std::cin >> i;
        init.setArchiver(i);
      } catch (...) {
        init.setArchiver(1);
      }
    }
    if (init.vcpkgPaths().size() == 0) {
      std::cout << "vcbld couldn't locate a vcpkg instance." << std::endl;
      init.setVcpkg();
    } else if (init.vcpkgPaths().size() == 1) {
      init.setVcpkg(1);
    } else {
      init.lister(init.vcpkgPaths());
      std::cout << "Please choose a vcpkg instance." << std::endl;
      try {
        int i;
        std::cin >> i;
        init.setVcpkg(i);
      } catch (...) {
        init.setVcpkg(1);
      }
    }
    init.setup();
  }
}

std::string parseQuotes(const char *arg) {
  std::string temp = static_cast<std::string>(arg);
  if (temp.find(" ") != std::string::npos) {
    return "\"" + temp + "\"";
  } else {
    return temp;
  }
}