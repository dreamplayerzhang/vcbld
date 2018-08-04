#include "pkg.h"

#include <errno.h>
#include <fstream>
#include <iostream>
#include <iterator>
#include <nlohmann/json.hpp>
#include <sstream>
#include <vector>

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

using json = nlohmann::json;

namespace vcbld {

PkgClass::PkgClass() {
  json pkgsJson;

  try {
    std::ifstream pkgsInput("packages.json");
    if (pkgsInput.is_open()) {
      pkgsJson = json::parse(pkgsInput);
      pkgsInput.close();
    } else {
      std::cerr << "Failed to open packages.json file : " << errno << std::endl;
    }
    for (json::iterator it = pkgsJson["packages"].begin();
         it != pkgsJson["packages"].end(); ++it) {
      this->packageName.push_back(*it);
    }
  } catch (const json::parse_error e) {
    std::cerr << "Error reading packages.json" << std::endl;
  }
}

void PkgClass::write() {
  nlohmann::json pkgsJson;

  for (std::vector<std::string>::iterator it = this->packageName.begin();
       it != this->packageName.end(); ++it) {
    pkgsJson["packages"].push_back((std::string)*it);
  }
  std::ofstream pkgsOutput("packages.json");
  if (pkgsOutput.is_open()) {
    pkgsOutput << std::setw(4) << pkgsJson;
    pkgsOutput.flush();
    pkgsOutput.close();
    std::cout << "Packages updated successfully." << std::endl;
  } else {
    std::cout << "Packages updated successfully." << std::endl;
    std::cerr << "Failed to open file : " << errno << std::endl;
  }
}

std::string PkgClass::getVersion(const std::string &pkgName) {
  std::string ctrlPath = confClass.vcpkgDirPath() + PATHSEP + "packages" +
                         PATHSEP + pkgName + "_" + confClass.architecture() +
                         PATHSEP + "CONTROL";
  std::string line;
  try {
    std::ifstream input(ctrlPath);
    if (input.is_open()) {
      std::getline(input, line);
      std::getline(input, line);

      input.close();
    } else {
      std::cerr << "Failed to open file : " << errno << std::endl;
    }
  } catch (const std::exception &e) {
    std::cout << "CONTROL file not found" << std::endl;
  }
  std::size_t found;
  std::string cropVersion;
  found = line.find(": ");
  if (found != std::string::npos) {
    cropVersion = line.substr((found + 2), line.length());
  }
  return cropVersion;
}

std::string PkgClass::headerPaths() {
  std::ostringstream temp;
  temp << " -I" << confClass.sourceDirectory() << " -I"
       << confClass.includeDirectory() << " -I" << confClass.vcpkgDirPath()
       << PATHSEP << "installed" << PATHSEP << confClass.architecture()
       << PATHSEP << "include";

  for (std::vector<std::string>::iterator it = this->packageName.begin();
       it != this->packageName.end(); ++it) {
    temp << " -I" << confClass.vcpkgDirPath() << PATHSEP << "packages"
         << PATHSEP << *it << "_" << confClass.architecture() << PATHSEP
         << "include ";
  }
  return temp.str();
}

std::string PkgClass::getLibName(const std::string &lib) {
  std::size_t found;
  std::string stripLib;
  std::string stripExt;
  found = lib.find("lib");
  if (found != std::string::npos && found == 0) {
    stripLib = lib.substr(3, lib.length());
  }
  found = stripLib.find(".a");
  if (found != std::string::npos) {
    stripExt = stripLib.substr(0, (stripLib.length() - 2));
  }
  found = stripLib.find(".so");
  if (found != std::string::npos) {
    stripExt = stripLib.substr(0, (stripLib.length() - 3));
  }
  found = stripLib.find(".dylib");
  if (found != std::string::npos) {
    stripExt = stripLib.substr(0, (stripLib.length() - 6));
  }
  found = stripLib.find(".dll");
  if (found != std::string::npos) {
    stripExt = stripLib.substr(0, (stripLib.length() - 4));
  }

  return stripExt;
}

std::string PkgClass::dbgLibPaths() {
  std::ostringstream temp;
  std::string localDbgLibs =
      confClass.libsDirectory().string() + PATHSEP + "debug";
  std::vector<fs::directory_entry> v;

  if (fs::is_directory((fs::path)localDbgLibs)) {
    std::copy(fs::directory_iterator(localDbgLibs), fs::directory_iterator(),
              std::back_inserter(v));

    for (std::vector<fs::directory_entry>::iterator jt = v.begin();
         jt != v.end(); ++jt) {
      if (getLibName((*jt).path().filename().string()).at(0) != '.') {
        temp << " -L" << (*jt).path().parent_path().string() << " "
             << " -l" << getLibName((*jt).path().filename().string());
      }
    }
  }

  for (std::vector<std::string>::iterator it = this->packageName.begin();
       it != this->packageName.end(); ++it) {
    std::string vcpkgDbgLibs = confClass.vcpkgDirPath() + PATHSEP + "packages" +
                               PATHSEP + *it + "_" + confClass.architecture() +
                               PATHSEP + "debug" + PATHSEP + "lib";
    if (fs::is_directory((fs::path)vcpkgDbgLibs)) {
      std::copy(fs::directory_iterator(vcpkgDbgLibs), fs::directory_iterator(),
                std::back_inserter(v));
      for (std::vector<fs::directory_entry>::iterator jt = v.begin();
           jt != v.end(); ++jt) {
        if (getLibName((*jt).path().filename().string()).at(0) != '.') {
          temp << " -L" << (*jt).path().parent_path().string() << " "
               << " -l" << getLibName((*jt).path().filename().string());
        }
      }
    }
  }
  return temp.str();
}
std::string PkgClass::rlsLibPaths() {
  std::ostringstream temp;
  std::string localRlsLibs =
      confClass.libsDirectory().string() + PATHSEP + "lib";
  std::vector<fs::directory_entry> v;

  if (fs::is_directory((fs::path)localRlsLibs)) {
    std::copy(fs::directory_iterator(localRlsLibs), fs::directory_iterator(),
              std::back_inserter(v));

    for (std::vector<fs::directory_entry>::iterator jt = v.begin();
         jt != v.end(); ++jt) {
      if (getLibName((*jt).path().filename().string()).at(0) != '.') {
        temp << " -L" << (*jt).path().parent_path().string() << " "
             << " -l" << getLibName((*jt).path().filename().string());
      }
    }
  }

  for (std::vector<std::string>::iterator it = this->packageName.begin();
       it != this->packageName.end(); ++it) {
    std::string vcpkgRlsLibs = confClass.vcpkgDirPath() + PATHSEP + "packages" +
                               PATHSEP + *it + "_" + confClass.architecture() +
                               PATHSEP + "lib";
    if (fs::is_directory((fs::path)vcpkgRlsLibs)) {
      std::copy(fs::directory_iterator(vcpkgRlsLibs), fs::directory_iterator(),
                std::back_inserter(v));
      for (std::vector<fs::directory_entry>::iterator jt = v.begin();
           jt != v.end(); ++jt) {
        if (getLibName((*jt).path().filename().string()).at(0) != '.') {
          temp << " -L" << (*jt).path().parent_path().string() << " "
               << " -l" << getLibName((*jt).path().filename().string());
        }
      }
    }
  }
  return temp.str();
}

void PkgClass::include(const std::string &pkgName) {
  this->packageName.push_back(pkgName);
}

void PkgClass::remove(const std::string &pkgName) {
  for (std::vector<std::string>::iterator it = this->packageName.begin();
       it != this->packageName.end();) {
    if (*it == pkgName) {
      it = this->packageName.erase(it);
    } else {
      ++it;
    }
  }
}

} // namespace vcbld