#include "args.h"

#include <cstdlib>
#include <cstring>
#include <errno.h>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <sstream>
#include <vector>

#include "builder.h"
#include "gen.h"
#include "help.h"
#include "init.h"
#include "pkg.h"

namespace fs = std::experimental::filesystem;

namespace vcbld {
namespace args {

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

  if (!fs::exists("lib")) {
    fs::create_directory("lib");
    std::cout << "lib directory created successfully." << std::endl;
  } else {
    std::cout << "lib directroy exists." << std::endl;
  }

  if (!fs::exists("./lib/debug")) {
    fs::create_directory("./lib/debug");
    std::cout << "lib/debug directory created successfully." << std::endl;
  } else {
    std::cout << "lib/debug directroy exists." << std::endl;
  }

  if (!fs::exists("./lib/release")) {
    fs::create_directory("./lib/release");
    std::cout << "lib/release directory created successfully." << std::endl;
  } else {
    std::cout << "lib/release directroy exists." << std::endl;
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
  init::init(binType);
}

void setup(const fs::path &vcbldPath) {
  init::setup(vcbldPath);
}

void build(const std::string &buildType) {
  Builder builder(buildType);
  builder.build();
}

void clean(const std::string &buildType) {
  try {
    ConfClass confClass;
    std::string cleanDir = confClass.outputDirectory() + "/" + buildType;
    if (fs::is_directory(cleanDir)) {
      std::vector<fs::directory_entry> dirEntry;
      std::copy(fs::directory_iterator(cleanDir), fs::directory_iterator(),
                back_inserter(dirEntry));
      for (std::vector<fs::directory_entry>::iterator it = dirEntry.begin();
           it != dirEntry.end(); ++it) {
        fs::remove_all(*it);
      }
    }
  } catch (const std::exception &e) {
    std::cout << "An error occured while deleting output." << std::endl;
    std::cerr << e.what() << std::endl;
  }
}

void run(const std::string &buildType) {
  std::ostringstream command;
  ConfClass confClass;
  std::string ext;
  if (confClass.architecture() == "x64-windows" ||
      confClass.architecture() == "x86-windows") {
    ext = ".exe";
  } else {
    ext = "";
  }
  if (buildType == "debug") {
    try {
      command << confClass.outputDirectory() << "/debug/"
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
      command << confClass.outputDirectory() << "/release/"
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
    std::vector<fs::directory_entry> dirEntry;

    std::string vcpkgDirPath = pkgClass.vcpkgDirPath();
    vcpkgDirPath += "/";
    vcpkgDirPath += "installed";
    vcpkgDirPath += "/";
    vcpkgDirPath += pkgClass.architecture();
    vcpkgDirPath += "/";
    vcpkgDirPath += "share";

    if (fs::is_directory(static_cast<fs::path>(vcpkgDirPath))) {
      std::copy(fs::directory_iterator(vcpkgDirPath), fs::directory_iterator(),
                back_inserter(dirEntry));

      for (std::vector<fs::directory_entry>::const_iterator it =
               dirEntry.begin();
           it != dirEntry.end(); ++it) {
        if ((it->path().filename().string()).at(0) != '.') {
          std::cout << (it->path().filename().string()) << std::endl;
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

    if (fs::is_directory(static_cast<fs::path>(vcpkgDirPath))) {
      std::copy(fs::directory_iterator(vcpkgDirPath), fs::directory_iterator(),
                back_inserter(dirEntry));

      for (std::vector<fs::directory_entry>::const_iterator it =
               dirEntry.begin();
           it != dirEntry.end(); ++it) {
        if ((it->path().filename().string()).find(pkg) != std::string::npos) {
          std::cout << (it->path().filename().string()) << std::endl;
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
  Builder dbgBuilder("debug");
  dbgBuilder.build();
  std::cout << dbgBuilder.getBldCommands() << std::endl;
  Builder rlsBuilder("release");
  rlsBuilder.build();
  std::cout << rlsBuilder.getBldCommands() << std::endl;
}

void list() {
  try {
    PkgClass pkgClass;
    if (pkgClass.packageNames().size() == 0) {
      std::cout << "No packages were added to your project!" << std::endl;
    }
    for (std::vector<std::string>::iterator it =
             pkgClass.packageNames().begin();
         it != pkgClass.packageNames().end(); ++it) {
      std::cout << std::setw(4) << "Package name: " << *it
                << "\t\tPackage version: " << pkgClass.getVersion(*it)
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
  for (std::vector<std::string>::const_iterator it = pkg.begin();
       it != pkg.end(); ++it) {
    std::string addDep = pkgClass.vcpkgDirPath() + "/" + "installed" + "/" +
                         pkgClass.architecture() + "/" + "share" + "/" + *it;

    if (fs::is_directory(static_cast<fs::path>(addDep))) {
      bool isExist = false;
      for (std::vector<std::string>::iterator jt =
               pkgClass.packageNames().begin();
           jt != pkgClass.packageNames().end(); ++jt) {
        if (*jt == *it) {
          std::cout << "Package already exists." << std::endl;
          isExist = true;
          break;
        } else {
          isExist = false;
        }
      }
      if (isExist != true) {
        pkgClass.include(*it);
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
  for (std::vector<std::string>::const_iterator it = pkg.begin();
       it != pkg.end(); ++it) {
    for (std::vector<std::string>::iterator jt =
             pkgClass.packageNames().begin();
         jt != pkgClass.packageNames().end(); ++jt) {
      if (*jt == *it) {
        pkgClass.remove(*it);
        pkgClass.write();
        break;
      } else {
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

void install(const std::string &packages) {
  ConfClass confClass;
  std::string temp =
      confClass.vcpkgDirPath() + "/" + "vcpkg" + " install " + packages;
  int systemRet = system(temp.c_str());
  if (systemRet == -1) {
    std::cout << "An error occured while installing." << std::endl;
  }
}

void uninstall(const std::string &packages) {
  ConfClass confClass;
  std::string temp =
      confClass.vcpkgDirPath() + "/" + "vcpkg" + " remove " + packages;
  int systemRet = system(temp.c_str());
  if (systemRet == -1) {
    std::cout << "An error occured while uninstalling." << std::endl;
  }
}

void restore() {
  PkgClass pkgClass;
  if (pkgClass.packageNames().size() != 0) {
    std::ostringstream pkg;
    for (std::vector<std::string>::iterator it =
             pkgClass.packageNames().begin();
         it != pkgClass.packageNames().end(); ++it) {
      pkg << *it << " ";
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
  std::ostringstream cmakeCmnd, temp;
  if (confClass.architecture().find("windows") == std::string::npos) {
    temp << " -DCMAKE_C_COMPILER=\"" << confClass.cCompilerPath() << "\""
         << " -DCMAKE_CXX_COMPILER=\"" << confClass.cppCompilerPath() << "\""
         << " -DCMAKE_MAKE_PROGRAM=\"" << confClass.makePath() << "\" ";
  } else {
    temp << " ";
  }
  cmakeCmnd << "cd " << confClass.outputDirectory() << " && \""
            << confClass.cmakePath() << "\""
            << " -DCMAKE_PREFIX_PATH=\"" << confClass.vcpkgDirPath()
            << "/installed/" << confClass.architecture() << "\"" << temp.str()
            << cmakeArgs << " .. ";
  int systemRet = system(cmakeCmnd.str().c_str());
  if (systemRet == -1) {
    std::cout << "An error occured while running cmake." << std::endl;
  }
}

void make(const std::string &makeArgs) {
  ConfClass confClass;
  std::ostringstream makeCmnd;
  makeCmnd << "cd " << confClass.outputDirectory() << " && "
           << " " << confClass.makePath();
  int systemRet = system(makeCmnd.str().c_str());
  if (systemRet == -1) {
    std::cout << "An error occured while running make." << std::endl;
  }
}
void help() {
  help::mainHelp();
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
} // namespace args
} // namespace vcbld
