#include "args.h"

#include <boost/filesystem.hpp>
#include <cstdlib>
#include <cstring>
#include <errno.h>
#include <fstream>
#include <iostream>
#include <iterator>
#include <nlohmann/json.hpp>
#include <sstream>
#include <thread>
#include <vector>

#include "builder.h"
#include "conf.h"
#include "init.h"
#include "pkg.h"

#if defined(_WIN32)
#define PLATFORM_NAME "x86-windows"
#define PATHSEP "\"
#elif defined(_WIN64)
#define PLATFORM_NAME "x64-windows"
#define PATHSEP "\"
#elif defined(__CYGWIN__) && !defined(_WIN32)
#define PLATFORM_NAME "x64-windows"
#define PATHSEP "/"
#elif defined(__linux__)
#define PLATFORM_NAME "x64-linux"
#define PATHSEP "/"
#elif defined(__APPLE__) && defined(__MACH__)
#include <TargetConditionals.h>
#if TARGET_OS_MAC == 1
#define PLATFORM_NAME "x64-osx" // Apple OSX
#define PATHSEP "/"
#endif
#else
#define PLATFORM_NAME NULL
#endif

namespace fs = boost::filesystem;
using json = nlohmann::json;

namespace vcbld::Args {

void New() {

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

  if (!fs::exists("libs")) {
    fs::create_directory("libs");
    std::cout << "libs directory created successfully." << std::endl;
  } else {
    std::cout << "libs directroy exists." << std::endl;
  }

  if (!fs::exists("./libs/debug")) {
    fs::create_directory("./libs/debug");
    std::cout << "libs/debug directory created successfully." << std::endl;
  } else {
    std::cout << "libs/debug directroy exists." << std::endl;
  }

  if (!fs::exists("./libs/release")) {
    fs::create_directory("./libs/release");
    std::cout << "libs/release directory created successfully." << std::endl;
  } else {
    std::cout << "libs/release directroy exists." << std::endl;
  }

  if (!fs::exists("bin")) {
    fs::create_directory("bin");
    std::cout << "bin directory created successfully." << std::endl;
  } else {
    std::cout << "bin directroy exists." << std::endl;
  }

  if (!fs::exists("./bin/debug")) {
    fs::create_directory("./bin/debug");
    std::cout << "bin/debug directory created successfully." << std::endl;
  } else {
    std::cout << "bin/debug directroy exists." << std::endl;
  }

  if (!fs::exists("./bin/release")) {
    fs::create_directory("bin/release");
    std::cout << "bin/release directory created successfully." << std::endl;
  } else {
    std::cout << "bin/release directroy exists." << std::endl;
  }

  if (!fs::exists("./src/main.cpp")) {
    std::ofstream ofs("./src/main.cpp");

    if (ofs.is_open()) {
      ofs << "#include <iostream>" << std::endl;
      ofs << "int main(int argc, char *argv[])\n{\n\tstd::cout  << \"Hello "
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

  init::init();
}

void configure() { init::init(); }

void build(const std::string &buildType) {
  Builder builder(buildType);
  builder.build();
}

void clean() {
  try {
    ConfClass confClass;
    std::string command = "rm -rf " + confClass.outputDirectory().string() +
                          PATHSEP + "debug" + PATHSEP + "**";
    system(command.c_str());
    command = "rm -rf " + confClass.outputDirectory().string() + PATHSEP +
              "release" + PATHSEP + "**";
    system(command.c_str());
  } catch (const std::exception &e) {
    std::cout << "vcbld.json not found!" << std::endl;
  }
}

void run(const std::string &buildType) {
  if (buildType == "debug") {
    try {
      ConfClass confClass;
      std::string command = "cd " + confClass.outputDirectory().string() +
                            " && " + "." + PATHSEP + "debug" + PATHSEP +
                            confClass.binaryName();
      system(command.c_str());
    } catch (const std::exception &e) {
      std::cout << "vcbld.json not found!" << std::endl;
    }
  } else {
    try {
      ConfClass confClass;
      std::string command = "cd " + confClass.outputDirectory().string() +
                            " && " + "." + PATHSEP + "release" + PATHSEP +
                            confClass.binaryName();
      system(command.c_str());
    } catch (const std::exception &e) {
      std::cout << "vcbld.json not found!" << std::endl;
    }
  }
}

void available() {
  try {
    ConfClass confClass;
    std::vector<fs::directory_entry> v;

    std::string vcpkgDirPath = confClass.vcpkgDirPath();
    vcpkgDirPath += PATHSEP;
    vcpkgDirPath += "packages";

    if (fs::is_directory((fs::path)vcpkgDirPath)) {
      std::copy(fs::directory_iterator(vcpkgDirPath), fs::directory_iterator(),
                back_inserter(v));

      for (std::vector<fs::directory_entry>::const_iterator it = v.begin();
           it != v.end(); ++it) {
        if (sansTriplet((*it).path().filename().string()).at(0) != '.') {
          std::cout << sansTriplet((*it).path().filename().string())
                    << std::endl;
        }
      }
    } else {
      std::cout << "vcpkg packages not found!" << std::endl;
      std::cout << "Please verify your vcpkg path." << std::endl;
    }
  } catch (const std::exception &e) {
    std::cout << "packages.json not found!" << std::endl;
  }
}

void search(const std::string &pkg) {
  try {
    ConfClass confClass;
    std::vector<fs::directory_entry> v;

    std::string vcpkgDirPath = confClass.vcpkgDirPath();
    vcpkgDirPath += PATHSEP;
    vcpkgDirPath += "packages";

    if (fs::is_directory((fs::path)vcpkgDirPath)) {
      std::copy(fs::directory_iterator(vcpkgDirPath), fs::directory_iterator(),
                back_inserter(v));

      for (std::vector<fs::directory_entry>::const_iterator it = v.begin();
           it != v.end(); ++it)
        if (sansTriplet((*it).path().filename().string()).find(pkg) !=
            std::string::npos) {
          std::cout << sansTriplet((*it).path().filename().string())
                    << std::endl;
        }
    } else {
      std::cout << "vcpkg packages not found!" << std::endl;
      std::cout << "Please verify your vcpkg path." << std::endl;
    }
  } catch (const std::exception &) {
  }
}

void generate() {
  ConfClass confClass;
  PkgClass pkgClass;
  std::ostringstream includePath;
  includePath << confClass.vcpkgDirPath() << PATHSEP << "installed" << PATHSEP
              << confClass.architecture() << PATHSEP << "include\n";
  for (std::vector<std::string>::iterator it = pkgClass.packageName.begin();
       it != pkgClass.packageName.end(); ++it) {
    includePath << confClass.vcpkgDirPath() << PATHSEP << "packages" << PATHSEP
                << *it << confClass.architecture() << PATHSEP << "include\n";
  }

  std::string temp =
      confClass.sourceDirectory().string() + PATHSEP + "includePath.txt";
  std::ofstream ofs(temp);

  if (ofs.is_open()) {
    ofs << includePath.str();
    ofs.flush();
    ofs.close();
  }
}

void list() {
  try {
    PkgClass pkgClass;
    for (std::vector<std::string>::iterator it = pkgClass.packageName.begin();
         it != pkgClass.packageName.end(); ++it) {
      std::cout << std::setw(4) << "Package name: " << *it
                << "\t\tPackage version: " << pkgClass.getVersion(*it)
                << std::endl;
    }
  } catch (const std::exception &e) {
    std::cout << "vcpkg packages not found!" << std::endl;
    std::cout << "Please verify your vcpkg path." << std::endl;
  }
}

void add(const std::string &pkg) {
  ConfClass confClass;
  PkgClass pkgClass;
  std::string addDep = confClass.vcpkgDirPath() + PATHSEP + "packages" +
                       PATHSEP + pkg + "_" + confClass.architecture();

  if (fs::is_directory((fs::path)addDep)) {
    bool isExist = false;
    for (std::vector<std::string>::iterator pt = pkgClass.packageName.begin();
         pt != pkgClass.packageName.end(); ++pt) {
      if (((*pt) == pkg)) {
        std::cout << "Package already exists" << std::endl;
        isExist = true;
        break;
      } else {
        isExist = false;
      }
    }
    if (isExist != true) {
      pkgClass.include(pkg);
      pkgClass.write();
    }
  } else {
    std::cout << "Package not found!" << std::endl;
    std::cout << "Please verify your vcpkg path." << std::endl;
  }
}

void remove(const std::string &pkg) {
  PkgClass pkgClass;
  for (std::vector<std::string>::iterator pt = pkgClass.packageName.begin();
       pt != pkgClass.packageName.end(); ++pt) {
    if (((*pt) == pkg)) {
      pkgClass.remove(pkg);
      pkgClass.write();
      break;
    } else {
    }
  }
}

void vcpkg(const std::string &vcpkgCmnds) {
  ConfClass confClass;
  std::string temp =
      confClass.vcpkgDirPath() + PATHSEP + "vcpkg" + " " + vcpkgCmnds;
  system(temp.c_str());
}

void restore() {
  ConfClass confClass;
  PkgClass pkgClass;
  std::ostringstream pkg;

  for (std::vector<std::string>::iterator it = pkgClass.packageName.begin();
       it != pkgClass.packageName.end(); ++it) {
    pkg << *it << " ";
  }
  std::string instlCmnd = confClass.vcpkgDirPath() + PATHSEP + "vcpkg" + " " +
                          "install" + " " + pkg.str();
  system(instlCmnd.c_str());
}

std::string sansTriplet(const std::string &pkg) {
  std::size_t found;
  std::string stripTrip;
  found = pkg.find("_");
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
} // namespace vcbld::Args
