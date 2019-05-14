#include "args.h"
#include "builder.h"
#include "conf.h"
#include "gen.h"
#include "help.h"
#include "pch.h"
#include "pkg.h"
#include "vcbld.h"

namespace vcbld {
namespace args {

void handleArgs(int argc, char **argv) {
  fs::path vcbldPath = argv[0];
  vcbldPath = vcbldPath.parent_path();

  std::vector<std::string> args;
  for (int i = 0; i < argc; i++) {
    args.push_back(argv[i]);
  }

  if (args.size() >= 2) {
    if (args[1] == "new") {
      Init init(vcbldPath);
      if (args.size() < 3) {
        New(BinType::Application);
        init.init(BinType::Application);
      } else {
        if (args[2] == "app") {
          New(BinType::Application);
          init.init(BinType::Application);
        } else if (args[2] == "dylib") {
          New(BinType::DynamicLibrary);
          init.init(BinType::DynamicLibrary);
        } else if (args[2] == "statlib") {
          New(BinType::StaticLibrary);
          init.init(BinType::StaticLibrary);
        } else {
          New(BinType::Application);
          init.init(BinType::Application);
        }
      }
      setup(init);
    } else if (args[1] == "setup") {
      Init init(vcbldPath);
      setup(init);
    } else if (args[1] == "restore") {
      restore();
    } else if (args[1] == "includes") {
      includes();
    } else if (args[1] == "gen") {
      generate();
    } else if (args[1] == "commands") {
      commands();
    } else if (args[1] == "build") {
      if (args.size() < 3) {
        build(Configuration::Debug);
      } else {
        if (args[2] == "release" || args[2] == "-r") {
          build(Configuration::Release);
        } else if (args[2] == "debug" || args[2] == "-g") {
          build(Configuration::Debug);
        }
      }
    } else if (args[1] == "clean") {
      if (args.size() < 3) {
        clean(Configuration::Debug);
      } else {
        if (args[2] == "release" || args[2] == "-r") {
          clean(Configuration::Release);
        } else if (args[2] == "debug" || args[2] == "-g") {
          clean(Configuration::Debug);
        }
      }
    } else if (args[1] == "all") {
      available();
    } else if (args[1] == "run") {
      if (args.size() < 3) {
        try {
          run(Configuration::Debug);
        } catch (...) {
          std::cout << "Build configuration or entry not available!"
                    << std::endl;
        }
      } else {
        if (args[2] == "release" || args[2] == "-r") {
          try {
            run(Configuration::Release);
          } catch (...) {
            std::cout << "Build configuration or entry not available!"
                      << std::endl;
          }
        } else if (args[2] == "debug" || args[2] == "-g") {
          try {
            run(Configuration::Debug);
          } catch (const std::exception &e) {
            std::cout << "Build configuration or entry not available!"
                      << std::endl;
            std::cerr << e.what() << std::endl;
          }
        }
      }
    } else if (args[1] == "find") {
      if (args.size() < 3) {
        std::cout << "Please enter a package name to search for." << std::endl;
      } else {
        find(args[2]);
      }
    } else if (args[1] == "add") {
      if (args.size() < 3) {
        std::cout << "Please enter a package name to add it." << std::endl;
      } else {
        std::vector<std::string> packages;
        for (int i = 2; i < args.size(); i++) {
          packages.push_back(args[i]);
        }
        add(packages);
      }
    } else if (args[1] == "remove") {
      if (args.size() < 3) {
        std::cout << "Please enter a package name to remove it." << std::endl;
      } else {
        std::vector<std::string> packages;
        for (int i = 2; i < args.size(); i++) {
          packages.push_back(args[i]);
        }
        remove(packages);
      }
    } else if (args[1] == "list") {
      list();
    } else if (args[1] == "help") {
      std::cout << "\nvcbld command line tools, version: \t"
                << vcbld_MAJOR_VERSION << "." << vcbld_MINOR_VERSION << "."
                << vcbld_PATCH_VERSION << std::endl;
      help();
    } else if (args[1] == "install") {
      if (args.size() < 3) {
        std::cout << "Please enter package names to install." << std::endl;
      } else {
        std::vector<std::string> packages;
        for (int i = 2; i < args.size(); i++) {
          packages.push_back(args[i]);
        }
        install(packages);
        add(packages);
      }
    } else if (args[1] == "search") {
      if (args.size() < 3) {
        std::cout << "Please enter a package names to search for." << std::endl;
      } else {
        search(args[2]);
      }
    } else if (args[1] == "uninstall") {
      if (args.size() < 3) {
        std::cout << "Please enter package names to uninstall." << std::endl;
      } else {
        std::vector<std::string> packages;
        for (int i = 2; i < args.size(); i++) {
          packages.push_back(args[i]);
        }
        remove(packages);
        uninstall(packages);
      }
    } else if (args[1] == "vcpkg") {
      if (args.size() < 3) {
        vcpkg("help");
      } else {
        std::string vcpkgArgs;
        for (int i = 2; i < args.size(); i++) {
          vcpkgArgs += " ";
          vcpkgArgs += args[i];
        }
        vcpkg(vcpkgArgs);
      }
    } else if (args[1] == "cmake") {
      std::string cmakeArgs;
      if (args.size() < 3) {
        cmake(cmakeArgs);
      } else {
        if (args[2] == "debug" || args[2] == "-g") {
          cmakeArgs += "-DCMAKE_BUILD_TYPE=Debug";
        } else if (args[2] == "release" || args[2] == "-r") {
          cmakeArgs += "-DCMAKE_BUILD_TYPE=Release";
        } else {
          cmakeArgs += parseQuotes(args[2].c_str());
        }
        if (args.size() >= 4)
          for (int i = 3; i < args.size(); i++) {
            cmakeArgs += " ";
            cmakeArgs += parseQuotes(args[i].c_str());
          }
        cmake(cmakeArgs);
      }
    } else if (args[1] == "make") {
      if (args.size() < 3) {
        std::string empty = " ";
        make(empty);
      } else {
        std::string makeArgs;
        for (int i = 2; i < args.size(); i++) {
          makeArgs += " ";
          makeArgs += args[i];
        }
        make(makeArgs);
      }
    } else if (args[2] == "--version") {
      std::cout << vcbld_MAJOR_VERSION << "." << vcbld_MINOR_VERSION << "."
                << vcbld_PATCH_VERSION << std::endl;
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
    if (init.cCompilers().empty()) {
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
    if (init.cppCompilers().empty()) {
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
    if (init.cmakePaths().empty()) {
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
    if (init.makePaths().empty()) {
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
    if (init.archiverPaths().empty()) {
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
    if (init.vcpkgPaths().empty()) {
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
  std::string temp = arg;
  if (temp.find(' ') != std::string::npos) {
    return "\"" + temp + "\"";
  } else {
    return temp;
  }
}

void New(const BinType &binType) {
  if (!fs::exists("src")) {
    fs::create_directory("src");
    std::cout << "src directory created successfully." << std::endl;
  } else {
    std::cout << "src directroy exists." << std::endl;
  }

  if (!fs::exists("include")) {
    fs::create_directory("include");
    std::cout << "include directory created successfully." << std::endl;
  } else {
    std::cout << "include directroy exists." << std::endl;
  }

  if (!fs::exists("lib")) {
    fs::create_directory("lib");
    std::cout << "lib directory created successfully." << std::endl;
  } else {
    std::cout << "lib directroy exists." << std::endl;
  }

  if (!fs::exists("lib/debug")) {
    fs::create_directory("lib/debug");
  } else {
  }

  if (!fs::exists("lib/release")) {
    fs::create_directory("lib/release");
  } else {
  }

  if (!fs::exists("bin")) {
    fs::create_directory("bin");
    std::cout << "bin directory created successfully." << std::endl;
  } else {
    std::cout << "bin directroy exists." << std::endl;
  }

  if (!fs::exists("bin/debug")) {
    fs::create_directory("bin/debug");
  } else {
  }

  if (!fs::exists("bin/release")) {
    fs::create_directory("bin/release");
  } else {
  }

  if (!fs::exists("src/main.cpp") && binType == BinType::Application) {
    std::ofstream ofs("./src/main.cpp");

    if (ofs.is_open()) {
      ofs << "#include <iostream>\n\n"
          << "int main(int argc, char *argv[])\n{\n\tstd::cout  << \"Hello "
             "World\"  << std::endl; \n\treturn 0;\n}"
          << std::endl;
      ofs.flush();
      ofs.close();
      std::cout << "main.cpp written successfully." << std::endl;
    } else {
      std::cerr << "Failed to open file : " << errno << std::endl;
    }
  } else {
    std::cout << "main.cpp already exists." << std::endl;
  }
}

void build(const Configuration &buildType) {
  Builder builder(buildType);
  builder.build();
}

void clean(const Configuration &config) {
  std::string buildType;
  if (config == Configuration::Debug) {
    buildType = "debug";
  } else {
    buildType = "release";
  }
  try {
    ConfClass confClass;
    std::string cleanDir = confClass.outputDirectory() + "/" + buildType;
    if (fs::is_directory(cleanDir)) {
      std::vector<fs::directory_entry> dirEntry;
      std::copy(fs::directory_iterator(cleanDir), fs::directory_iterator(),
                back_inserter(dirEntry));
      for (auto &it : dirEntry) {
        fs::remove_all(it);
      }
      std::cout << "Cleaned build output." << std::endl;
    }
  } catch (const std::exception &e) {
    std::cout << "An error occured while deleting output." << std::endl;
    std::cerr << e.what() << std::endl;
  }
}

void run(const Configuration &buildType) {
  std::ostringstream command;
  ConfClass confClass;
  std::string ext, node, nodeExec;
  if (confClass.architecture() == "x64-windows" ||
      confClass.architecture() == "x86-windows") {
    ext = ".exe";
    node = "node.exe";
  } else {
    ext = "";
    node = "node";
  }

  if (confClass.binaryName().find(".js") != std::string::npos &&
      (confClass.compilerPath().find("emcc") != std::string::npos ||
       confClass.compilerPath().find("em++") != std::string::npos)) {
    nodeExec = node + " ";
  } else {
    nodeExec = "";
  }

  if (buildType == Configuration::Debug) {
    try {
      command << nodeExec << confClass.outputDirectory() << "/debug/"
              << confClass.binaryName() << ext;
      int systemRet = system(command.str().c_str());
      if (systemRet == -1) {
        std::cout << "An error occured while running the binary." << std::endl;
      }
    } catch (const std::exception &e) {
      std::cout << "An error occured while running the binary." << std::endl;
      std::cerr << e.what() << std::endl;
    }
  } else {
    try {
      command << nodeExec << confClass.outputDirectory() << "/release/"
              << confClass.binaryName() << ext;
      int systemRet = system(command.str().c_str());
      if (systemRet == -1) {
        std::cout << "An error occured while running the binary." << std::endl;
      }
    } catch (const std::exception &e) {
      std::cout << "An error occured while running the binary." << std::endl;
      std::cerr << e.what() << std::endl;
    }
  }
}

void available() {
  try {
    PkgClass pkgClass;
    std::set<fs::directory_entry> dirEntry;

    std::string vcpkgDirPath = pkgClass.vcpkgDirPath();
    vcpkgDirPath += "/";
    vcpkgDirPath += "installed";
    vcpkgDirPath += "/";
    vcpkgDirPath += pkgClass.architecture();
    vcpkgDirPath += "/";
    vcpkgDirPath += "share";

    if (fs::is_directory(vcpkgDirPath)) {
		std::copy(fs::directory_iterator(vcpkgDirPath), fs::directory_iterator(),
			std::inserter(dirEntry, dirEntry.begin()));

      for (auto &it : dirEntry) {
        if ((it.path().filename().string()).at(0) != '.') {
          std::cout << (it.path().filename().string()) << std::endl;
        }
      }
    } else {
      std::cout << "vcpkg packages not found!" << std::endl;
      std::cout << "Please verify your vcpkg path." << std::endl;
    }
  } catch (const std::exception &e) {
    std::cout << "package.json not found!" << std::endl;
    std::cerr << e.what() << std::endl;
  }
}

void find(const std::string &pkg) {
  try {
    PkgClass pkgClass;
    std::vector<fs::directory_entry> dirEntry;

    std::string vcpkgDirPath = pkgClass.vcpkgDirPath();
    vcpkgDirPath += "/";
    vcpkgDirPath += "installed";
    vcpkgDirPath += "/";
    vcpkgDirPath += pkgClass.architecture();
    vcpkgDirPath += "/";
    vcpkgDirPath += "share";

    if (fs::is_directory(vcpkgDirPath)) {
      std::copy(fs::directory_iterator(vcpkgDirPath), fs::directory_iterator(),
                back_inserter(dirEntry));

      for (auto &it : dirEntry) {
        if ((it.path().filename().string()).find(pkg) != std::string::npos) {
          std::cout << (it.path().filename().string()) << std::endl;
        }
      }
    } else {
      std::cout << "vcpkg packages not found!" << std::endl;
      std::cout << "Please verify your vcpkg path." << std::endl;
    }
  } catch (const std::exception &) {
    // fail quietly!
  }
}

void includes() { gen::includePathGen(); }
void generate() { gen::cmakeGen(); }

void commands() {
  Builder dbgBuilder(Configuration::Debug);
  std::cout << "\nDebug\n" << dbgBuilder.getBldCommands() << std::endl;
  Builder rlsBuilder(Configuration::Release);
  std::cout << "\nRelease\n"
            << rlsBuilder.getBldCommands() << "\n"
            << std::endl;
}

void list() {
  try {
    PkgClass pkgClass;
    if (pkgClass.packageNames().empty()) {
      std::cout << "No packages were added to your project!" << std::endl;
    }
    for (auto &it : pkgClass.packageNames()) {
      std::cout << std::setw(4) << "Package name: " << it
                << "\nPackage version: " << pkgClass.getVersion(it)
                << std::endl;
    }
  } catch (const std::exception &e) {
    std::cout << "vcpkg packages not found!" << std::endl;
    std::cout << "Please verify your vcpkg path." << std::endl;
    std::cerr << e.what() << std::endl;
  }
}

void add(const std::vector<std::string> &pkg) {
  PkgClass pkgClass;
  for (auto &it : pkg) {
    std::string addDep = pkgClass.vcpkgDirPath() + "/" + "installed" + "/" +
                         pkgClass.architecture() + "/" + "share" + "/" + it;

    if (fs::is_directory(addDep)) {
      bool isExist = false;
      for (auto &jt : pkgClass.packageNames()) {
        if (jt == it) {
          std::cout << "Package already exists." << std::endl;
          isExist = true;
          break;
        }
        isExist = false;
      }
      if (!isExist) {
        pkgClass.include(it);
        pkgClass.write();
      }
    } else {
      std::cout << "Package not found!" << std::endl;
      std::cout << "Please verify your vcpkg path." << std::endl;
    }
  }
}

void remove(const std::vector<std::string> &pkg) {
  PkgClass pkgClass;
  for (auto &it : pkg) {
    for (auto &jt : pkgClass.packageNames()) {
      if (jt == it) {
        pkgClass.remove(it);
        pkgClass.write();
        break;
      }
    }
  }
}

void vcpkg(const std::string &vcpkgCmnds) {
  ConfClass confClass;
  std::string temp =
      confClass.vcpkgDirPath() + "/" + "vcpkg" + " " + vcpkgCmnds;
  int systemRet = system(temp.c_str());
  if (systemRet == -1) {
    std::cout << "An error occured while running vcpkg." << std::endl;
  }
}

void search(const std::string &pkg) {
  ConfClass confClass;
  std::string temp =
      confClass.vcpkgDirPath() + "/" + "vcpkg" + " search " + pkg;
  int systemRet = system(temp.c_str());
  if (systemRet == -1) {
    std::cout << "An error occured while searching for packages." << std::endl;
  }
}

void install(std::vector<std::string> &pkg) {
  ConfClass confClass;
  std::string packages;
  for (auto &it : pkg) {
    packages += it + ':' + confClass.architecture() + " ";
  }
  std::string temp =
      confClass.vcpkgDirPath() + "/" + "vcpkg" + " install " + packages;
  int systemRet = system(temp.c_str());
  if (systemRet == -1) {
    std::cout << "An error occured while installing." << std::endl;
  }
}

void uninstall(std::vector<std::string> &pkg) {
  ConfClass confClass;
  std::string packages;
  for (auto &it : pkg) {
    packages += it + ':' + confClass.architecture() + " ";
  }
  std::string temp =
      confClass.vcpkgDirPath() + "/" + "vcpkg" + " remove " + packages;
  int systemRet = system(temp.c_str());
  if (systemRet == -1) {
    std::cout << "An error occured while uninstalling." << std::endl;
  }
}

void restore() {
  PkgClass pkgClass;
  if (!pkgClass.packageNames().empty()) {
    std::ostringstream pkg;
    for (auto &it : pkgClass.packageNames()) {
      pkg << it << ':' << pkgClass.architecture() << " ";
    }
    std::string instlCmnd = pkgClass.vcpkgDirPath() + "/" + "vcpkg" + " " +
                            "install" + " " + pkg.str();
    int systemRet = system(instlCmnd.c_str());
    if (systemRet == -1) {
      std::cout << "An error occured while getting missing dependencies."
                << std::endl;
    }
  } else {
    std::cout << "No packages were found in package.json" << std::endl;
  }
}

void cmake(const std::string &cmakeArgs) {
  ConfClass confClass;
  std::string emcmakePath;
  std::ostringstream cmakeCmnd, temp;

  if (confClass.compilerPath().find("emcc") != std::string::npos ||
      confClass.compilerPath().find("em++") != std::string::npos) {
    emcmakePath =
        (fs::canonical(confClass.compilerPath()).parent_path() / "emcmake")
            .string();
  }

  if (!emcmakePath.empty()) {
    if (PLATFORM_NAME == "x86-windows" || PLATFORM_NAME == "x64-windows") {
      emcmakePath = "\"" + emcmakePath + ".bat\" ";
    } else {
      emcmakePath = "\"" + emcmakePath + "\" ";
    }
  }

  temp << " -DCMAKE_C_COMPILER=\"" << confClass.cCompilerPath() << "\""
       << " -DCMAKE_CXX_COMPILER=\"" << confClass.cppCompilerPath() << "\""
       << " -DCMAKE_MAKE_PROGRAM=\"" << confClass.makePath() << "\" ";

  cmakeCmnd << "cd \"" << confClass.outputDirectory() << "\""
            << " && " << emcmakePath << "\"" << confClass.cmakePath() << "\""
            << " -DVCPKG_TARGET_TRIPLET=" << confClass.architecture()
            << " -DCMAKE_TOOLCHAIN_FILE=\"" << confClass.vcpkgDirPath()
            << "/scripts/buildsystems/vcpkg.cmake"
            << "\"" << temp.str() << cmakeArgs << " .. ";
  int systemRet = system(cmakeCmnd.str().c_str());
  if (systemRet == -1) {
    std::cout << "An error occured while running cmake." << std::endl;
  }
}

void make(const std::string &makeArgs) {
  ConfClass confClass;
  std::string emmakePath;
  std::ostringstream makeCmnd;

  if (confClass.compilerPath().find("emcc") != std::string::npos ||
      confClass.compilerPath().find("em++") != std::string::npos) {
    emmakePath =
        (fs::canonical(confClass.compilerPath()).parent_path() / "emmake")
            .string();
  }

  if (!emmakePath.empty()) {
    if (PLATFORM_NAME == "x86-windows" || PLATFORM_NAME == "x64-windows") {
      emmakePath = "\"" + emmakePath + ".bat\" ";
    } else {
      emmakePath = "\"" + emmakePath + "\" ";
    }
  }

  makeCmnd << "cd \"" << confClass.outputDirectory() << "\""
           << " && " << emmakePath << "\"" << confClass.makePath() << "\" "
           << makeArgs;
  int systemRet = system(makeCmnd.str().c_str());
  if (systemRet == -1) {
    std::cout << "An error occured while running make." << std::endl;
  }
}
void help() { help::mainHelp(); }

std::string sinTriplet(const std::string &pkg) {
  std::size_t found;
  std::string stripTrip;
  found = pkg.find('_');
  if (found != std::string::npos) {
    stripTrip = pkg.substr(0, found);
  }
  return stripTrip;
}

bool findPackage(const std::string &pkg) {
  std::size_t found;
  std::string pkgName;
  found = pkg.find(pkg);
  if (found != std::string::npos) {
    std::cout << found;
    return true;
  } else {
    return false;
  }
}
} // namespace args
} // namespace vcbld
