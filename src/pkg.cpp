#include "pkg.h"

#include <algorithm>
#include <errno.h>
#if defined(_WIN32) || defined(_WIN64)
#include <filesystem>
#else
#include <experimental/filesystem>
#endif
#include <fstream>
#include <iostream>
#include <iterator>
#include <nlohmann/json.hpp>

#include "init.h"

using json = nlohmann::json;
namespace fs = std::experimental::filesystem;

namespace vcbld {

PkgClass::PkgClass() : ConfClass() {
  json pkgsJson;
  try {
    std::ifstream pkgsInput("package.json");
    if (pkgsInput.is_open()) {
      pkgsJson = json::parse(pkgsInput);
      pkgsInput.close();
    } else {
      std::cerr << "Failed to open package.json file: " << errno << std::endl;
    }
    for (json::iterator it = pkgsJson["packages"].begin();
         it != pkgsJson["packages"].end(); ++it) {
      _packageNames.push_back(*it);
    }
  } catch (...) {
    std::cerr << "Error reading package.json." << std::endl;
  }
}

void PkgClass::write() {
  json pkgsJson;

  for (std::vector<std::string>::iterator it = _packageNames.begin();
       it != _packageNames.end(); ++it) {
    pkgsJson["packages"].push_back(*it);
  }
  try {
    std::ofstream pkgsOutput("package.json");
    if (pkgsOutput.is_open()) {
      pkgsOutput << std::setw(4) << pkgsJson;
      pkgsOutput.flush();
      pkgsOutput.close();
      std::cout << "Packages updated successfully." << std::endl;
    } else {
      std::cout << "Packages updated successfully." << std::endl;
    }
  } catch (const std::exception &e) {
    std::cerr << "Failed to open file: " << e.what() << " " << errno
              << std::endl;
  }
  std::string line;
  bool isEmpty = false;
  try {
    std::ifstream pkgsInput("package.json");
    if (pkgsInput.is_open()) {
      std::getline(pkgsInput, line);
      if (line == "null") {
        isEmpty = true;
        pkgsInput.close();
      }
    } else {
      std::cout << "Packages updated successfully." << std::endl;
    }
  } catch (const std::exception &e) {
    std::cerr << "Failed to open file : " << e.what() << " " << errno
              << std::endl;
  }
  if (isEmpty == true) {
    fs::remove("package.json");
    init::init(binaryType());
  }
}

std::string PkgClass::getVersion(const std::string &pkgName) {
  std::vector<fs::directory_entry> dirEntry;
  std::string tempPath =
      vcpkgDirPath() + "/" + "installed" + "/" + +"vcpkg" + "/" + "info";
  std::string temp, temp2;
  size_t foundArch, foundPkg;
  std::string arch = "_" + architecture() + ".list";
  std::string pkg = pkgName + "_";
  if (fs::is_directory(static_cast<fs::path>(tempPath))) {
    std::copy(fs::directory_iterator(tempPath), fs::directory_iterator(),
              back_inserter(dirEntry));
    std::sort(dirEntry.begin(), dirEntry.end());
    dirEntry.erase(std::unique(dirEntry.begin(), dirEntry.end()),
                   dirEntry.end());
    for (std::vector<fs::directory_entry>::iterator it = dirEntry.begin();
         it != dirEntry.end(); ++it) {
      std::string fileName = it->path().filename().string();
      foundPkg = fileName.find(pkg);
      if (foundPkg != std::string::npos && foundPkg == 0) {
        temp2 = it->path().filename().string().substr(pkg.length(),
                                                        fileName.length());
      }
    }
  }
  foundArch = temp2.find(arch);
  temp = temp2.substr(0, foundArch);
  return temp;
}

void PkgClass::include(const std::string &pkgName) {
  _packageNames.push_back(pkgName);
}

void PkgClass::remove(const std::string &pkgName) {
  for (std::vector<std::string>::iterator it = _packageNames.begin();
       it != _packageNames.end();) {
    if (*it == pkgName) {
      it = _packageNames.erase(it);
    } else {
      ++it;
    }
  }
}

std::vector<std::string> &PkgClass::packageNames() {
  return _packageNames;
}
} // namespace vcbld
