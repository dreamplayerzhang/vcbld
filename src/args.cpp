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
#include "gen.h"
#include "init.h"

namespace fs = boost::filesystem;
using json = nlohmann::json;

namespace vcbld::args {

void New(const std::string &binType) {

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

  if (!fs::exists("./src/main.cpp") && binType == "app") {
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

  init::init(binType);
}

void configure(const fs::path &vcbldPath) { ConfClass confClass(vcbldPath); }

void build(const std::string &buildType, const fs::path &vcbldPath) {
  Builder builder(buildType, vcbldPath);
  builder.build();
}

void clean(const fs::path &vcbldPath) {
  try {
    ConfClass confClass(vcbldPath);
    std::string command = "rm -rf " + confClass.outputDirectory().string() +
                          "/" + "debug" + "/" + "**";
    system(command.c_str());
    command = "rm -rf " + confClass.outputDirectory().string() + "/" +
              "release" + "/" + "**";
    system(command.c_str());
  } catch (const std::exception &e) {
    std::cout << "vcbld.json not found!" << std::endl;
  }
}

void run(const std::string &buildType, const fs::path &vcbldPath) {
  if (buildType == "debug") {
    try {
      ConfClass confClass(vcbldPath);
      std::string command = "cd " + confClass.outputDirectory().string() +
                            " && " + "." + "/" + "debug" + "/" +
                            confClass.binaryName();
      system(command.c_str());
    } catch (const std::exception &e) {
      std::cout << "vcbld.json not found!" << std::endl;
    }
  } else {
    try {
      ConfClass confClass(vcbldPath);
      std::string command = "cd " + confClass.outputDirectory().string() +
                            " && " + "." + "/" + "release" + "/" +
                            confClass.binaryName();
      system(command.c_str());
    } catch (const std::exception &e) {
      std::cout << "vcbld.json not found!" << std::endl;
    }
  }
}

void available(const fs::path &vcbldPath) {
  try {
    ConfClass confClass(vcbldPath);
    std::vector<fs::directory_entry> v;

    std::string vcpkgDirPath = confClass.vcpkgDirPath();
    vcpkgDirPath += "/";
    vcpkgDirPath += "packages";

    if (fs::is_directory(static_cast<fs::path>(vcpkgDirPath))) {
      std::copy(fs::directory_iterator(vcpkgDirPath), fs::directory_iterator(),
                back_inserter(v));

      for (std::vector<fs::directory_entry>::const_iterator it = v.begin();
           it != v.end(); ++it) {
        if (sinTriplet((*it).path().filename().string()).at(0) != '.') {
          std::cout << sinTriplet((*it).path().filename().string())
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

void search(const std::string &pkg, const fs::path &vcbldPath) {
  try {
    ConfClass confClass(vcbldPath);
    std::vector<fs::directory_entry> v;

    std::string vcpkgDirPath = confClass.vcpkgDirPath();
    vcpkgDirPath += "/";
    vcpkgDirPath += "packages";

    if (fs::is_directory(static_cast<fs::path>(vcpkgDirPath))) {
      std::copy(fs::directory_iterator(vcpkgDirPath), fs::directory_iterator(),
                back_inserter(v));

      for (std::vector<fs::directory_entry>::const_iterator it = v.begin();
           it != v.end(); ++it)
        if (sinTriplet((*it).path().filename().string()).find(pkg) !=
            std::string::npos) {
          std::cout << sinTriplet((*it).path().filename().string())
                    << std::endl;
        }
    } else {
      std::cout << "vcpkg packages not found!" << std::endl;
      std::cout << "Please verify your vcpkg path." << std::endl;
    }
  } catch (const std::exception &) {
  }
}

void generate(const fs::path &vcbldPath) {
  gen::includePathGen(vcbldPath);
  gen::cmakeGen(vcbldPath);
}

void list(const fs::path &vcbldPath) {
  try {
    ConfClass confClass(vcbldPath);
    for (std::vector<std::string>::iterator it = confClass.packageName.begin();
         it != confClass.packageName.end(); ++it) {
      std::cout << std::setw(4) << "Package name: " << *it
                << "\t\tPackage version: " << confClass.getVersion(*it)
                << std::endl;
    }
  } catch (const std::exception &e) {
    std::cout << "vcpkg packages not found!" << std::endl;
    std::cout << "Please verify your vcpkg path." << std::endl;
  }
}

void add(const std::string &pkg, const fs::path &vcbldPath) {
  ConfClass confClass(vcbldPath);
  std::string addDep = confClass.vcpkgDirPath() + "/" + "packages" + "/" + pkg +
                       "_" + confClass.architecture();

  if (fs::is_directory(static_cast<fs::path>(addDep))) {
    bool isExist = false;
    for (std::vector<std::string>::iterator pt = confClass.packageName.begin();
         pt != confClass.packageName.end(); ++pt) {
      if (((*pt) == pkg)) {
        std::cout << "Package already exists" << std::endl;
        isExist = true;
        break;
      } else {
        isExist = false;
      }
    }
    if (isExist != true) {
      confClass.include(pkg);
      confClass.write();
    }
  } else {
    std::cout << "Package not found!" << std::endl;
    std::cout << "Please verify your vcpkg path." << std::endl;
  }
}

void remove(const std::string &pkg, const fs::path &vcbldPath) {
  ConfClass confClass(vcbldPath);
  for (std::vector<std::string>::iterator pt = confClass.packageName.begin();
       pt != confClass.packageName.end(); ++pt) {
    if (((*pt) == pkg)) {
      confClass.remove(pkg);
      confClass.write();
      break;
    } else {
    }
  }
}

void vcpkg(const std::string &vcpkgCmnds, const fs::path &vcbldPath) {
  ConfClass confClass(vcbldPath);
  std::string temp =
      confClass.vcpkgDirPath() + "/" + "vcpkg" + " " + vcpkgCmnds;
  system(temp.c_str());
}

void restore(const fs::path &vcbldPath) {
  ConfClass confClass(vcbldPath);
  std::ostringstream pkg;

  for (std::vector<std::string>::iterator it = confClass.packageName.begin();
       it != confClass.packageName.end(); ++it) {
    pkg << *it << " ";
  }
  std::string instlCmnd = confClass.vcpkgDirPath() + "/" + "vcpkg" + " " +
                          "install" + " " + pkg.str();
  system(instlCmnd.c_str());
}

void cmake(const std::string &cmakeArgs, const fs::path &vcbldPath) {
  ConfClass confClass(vcbldPath);
  std::ostringstream cmakeCmnd;
  cmakeCmnd << "cd " << confClass.outputDirectory() << " && "
            << confClass.cmakePath() << " "
            << "-DCMAKE_TOOL_CHAIN_FILE=" << confClass.vcpkgDirPath()
            << "/scripts/buildsystems/vcpkg.cmake " << " " << cmakeArgs << " .. ";
  system(cmakeCmnd.str().c_str());
}

std::string sinTriplet(const std::string &pkg) {
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
} // namespace vcbld::args
