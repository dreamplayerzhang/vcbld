#include "prep.h"

#include <algorithm>
#include <errno.h>
#if defined(_WIN32)
#include <filesystem>
#else
#include <experimental/filesystem>
#endif
#include <fstream>
#include <iostream>
#include <iterator>

namespace fs = std::experimental::filesystem;

namespace vcbld {
PrepClass::PrepClass() : PkgClass() {
  std::vector<fs::directory_entry> dirEntry;
  std::string arch = "_" + this->architecture() + ".list";
  std::string temp, temp2;
  size_t foundFile;
  std::string listsPath =
      this->vcpkgDirPath() + "/" + "installed" + "/" + +"vcpkg" + "/" + "info";
  if (fs::is_directory(static_cast<fs::path>(listsPath))) {
    std::copy(fs::directory_iterator(listsPath), fs::directory_iterator(),
              back_inserter(dirEntry));
    std::sort(dirEntry.begin(), dirEntry.end());
    dirEntry.erase(std::unique(dirEntry.begin(), dirEntry.end()),
                   dirEntry.end());
    for (std::vector<fs::directory_entry>::iterator it = dirEntry.begin();
         it != dirEntry.end(); ++it) {

      for (std::vector<std::string>::iterator jt = this->packageNames().begin();
           jt != this->packageNames().end(); ++jt) {
        foundFile = (*it).path().filename().string().find(*jt);
        if (foundFile != std::string::npos && foundFile == 0) {
          std::string fileName = (*it).path().filename().string();
          std::string line;
          try {
            std::ifstream ifs((*it).path());
            if (ifs.is_open()) {
              while (!ifs.eof()) {
                getline(ifs, line);
                if (findLib(line) != "") {
                  std::string libFound = findLib(line);
                  this->_fullLibNames.push_back(libFound);
                }
              }
            }
          } catch (...) {
            // fail quietly!
          }
        }
        std::sort(this->_fullLibNames.begin(), this->_fullLibNames.end());
        this->_fullLibNames.erase(
            std::unique(this->_fullLibNames.begin(), this->_fullLibNames.end()),
            this->_fullLibNames.end());
      }
    }
  }

  if (this->libDirectory().string() != "") {
    std::string localDbgLibs = this->libDirectory().string() + "/" + "debug";
    std::vector<fs::directory_entry> localDbgDirEntry;

    if (fs::is_directory(static_cast<fs::path>(localDbgLibs))) {
      std::copy(fs::directory_iterator(localDbgLibs), fs::directory_iterator(),
                std::back_inserter(localDbgDirEntry));
      std::sort(localDbgDirEntry.begin(), localDbgDirEntry.end());
      localDbgDirEntry.erase(
          std::unique(localDbgDirEntry.begin(), localDbgDirEntry.end()),
          localDbgDirEntry.end());

      for (std::vector<fs::directory_entry>::iterator jt =
               localDbgDirEntry.begin();
           jt != localDbgDirEntry.end(); ++jt) {
        if (((*jt).path().filename().string()).at(0) != '.') {
          this->_dbgLocalLibNames.push_back((*jt).path().filename().string());
        }
      }
    }

    std::string localRlsLibs = this->libDirectory().string() + "/" + "release";
    std::vector<fs::directory_entry> localRlsDirEntry;

    if (fs::is_directory(static_cast<fs::path>(localRlsLibs))) {
      std::copy(fs::directory_iterator(localRlsLibs), fs::directory_iterator(),
                std::back_inserter(localRlsDirEntry));
      std::sort(localRlsDirEntry.begin(), localRlsDirEntry.end());
      localRlsDirEntry.erase(
          std::unique(localRlsDirEntry.begin(), localRlsDirEntry.end()),
          localRlsDirEntry.end());

      for (std::vector<fs::directory_entry>::iterator jt =
               localRlsDirEntry.begin();
           jt != localRlsDirEntry.end(); ++jt) {
        if (((*jt).path().filename().string()).at(0) != '.') {
          this->_rlsLocalLibNames.push_back((*jt).path().filename().string());
        }
      }
    }
  }
}

std::string PrepClass::sourceFiles() const {
  std::vector<fs::directory_entry> dirEntry;
  std::string tempPath;
  std::ostringstream temp;
  tempPath = fs::canonical(this->sourceDirectory()).string();
  if (fs::is_directory(static_cast<fs::path>(tempPath))) {
    std::copy(fs::directory_iterator(tempPath), fs::directory_iterator(),
              back_inserter(dirEntry));
    std::sort(dirEntry.begin(), dirEntry.end());
    dirEntry.erase(std::unique(dirEntry.begin(), dirEntry.end()),
                   dirEntry.end());

    for (std::vector<fs::directory_entry>::iterator it = dirEntry.begin();
         it != dirEntry.end(); ++it) {
      if (fs::path((*it).path().filename().string()).extension() == ".cpp" ||
          fs::path((*it).path().filename().string()).extension() == ".rc" ||
          fs::path((*it).path().filename().string()).extension() == ".c" ||
          fs::path((*it).path().filename().string()).extension() == ".cxx" ||
          fs::path((*it).path().filename().string()).extension() == ".qrc" ||
          fs::path((*it).path().filename().string()).extension() == ".uic") {
        temp << this->projPath().c_str() << "/" << tempPath + "/"
             << (*it).path().filename().string() << " ";
      }
    }
  }
  return temp.str();
}

std::string PrepClass::sourceFilesSinPath() const {
  std::vector<fs::directory_entry> dirEntry;
  std::string tempPath;
  std::ostringstream temp;
  tempPath = fs::canonical(this->sourceDirectory()).string();
  if (fs::is_directory(static_cast<fs::path>(tempPath))) {
    std::copy(fs::directory_iterator(tempPath), fs::directory_iterator(),
              back_inserter(dirEntry));
    std::sort(dirEntry.begin(), dirEntry.end());
    dirEntry.erase(std::unique(dirEntry.begin(), dirEntry.end()),
                   dirEntry.end());

    for (std::vector<fs::directory_entry>::iterator it = dirEntry.begin();
         it != dirEntry.end(); ++it) {
      if (fs::path((*it).path().filename().string()).extension() == ".cpp" ||
          fs::path((*it).path().filename().string()).extension() == ".rc" ||
          fs::path((*it).path().filename().string()).extension() == ".c" ||
          fs::path((*it).path().filename().string()).extension() == ".cxx" ||
          fs::path((*it).path().filename().string()).extension() == ".qrc" ||
          fs::path((*it).path().filename().string()).extension() == ".uic") {
        temp << "\"" << (*it).path().filename().string() << "\" ";
      }
    }
  }
  return temp.str();
}

std::string PrepClass::headerPaths() {
  std::ostringstream temp;
  temp << " -I" << fs::canonical(this->sourceDirectory()).string() << " -I"
       << fs::canonical(this->outputDirectory()).string();
  if (this->includeDirectory() != "") {
    temp << " -I" << fs::canonical(this->includeDirectory()).string();
  }
  temp << " -I" << this->vcpkgDirPath() << "/"
       << "installed"
       << "/" << this->architecture() << "/"
       << "include";
  return temp.str();
}

std::string PrepClass::stripLibName(const std::string &lib) {
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
  found = stripLib.find(".lib");
  if (found != std::string::npos) {
    stripExt = stripLib.substr(0, (stripLib.length() - 4));
  }

  return stripExt;
}

std::string PrepClass::dbgLibPaths() {
  std::ostringstream temp;
  std::string localDbgLibs = this->libDirectory().string() + "/" + "debug";
  for (std::vector<std::string>::iterator jt = this->_dbgLocalLibNames.begin();
       jt != this->_dbgLocalLibNames.end(); ++jt) {
    temp << " -L" << fs::canonical(localDbgLibs).string() << " "
         << " -l" << stripLibName(*jt);
  }

  std::string dbgLibPath = this->vcpkgDirPath() + "/" + "installed" + "/" +
                           this->architecture() + "/" + "debug" + "/" + "lib";
  for (std::vector<std::string>::iterator it = this->_fullLibNames.begin();
       it != this->_fullLibNames.end(); ++it) {
    temp << " -L" << dbgLibPath << " "
         << "-l" << stripLibName(*it);
  }
  return temp.str();
}

std::string PrepClass::rlsLibPaths() {
  std::ostringstream temp;
  std::string localRlsLibs = this->libDirectory().string() + "/" + "release";
  for (std::vector<std::string>::iterator jt = this->_rlsLocalLibNames.begin();
       jt != this->_rlsLocalLibNames.end(); ++jt) {
    temp << " -L" << fs::canonical(localRlsLibs).string() << " "
         << " -l" << stripLibName(*jt);
  }

  std::string dbgLibPath = this->vcpkgDirPath() + "/" + "installed" + "/" +
                           this->architecture() + "/" + "lib";
  for (std::vector<std::string>::iterator it = this->_fullLibNames.begin();
       it != this->_fullLibNames.end(); ++it) {
    temp << " -L" << dbgLibPath << " "
         << "-l" << stripLibName(*it);
  }
  return temp.str();
}

bool PrepClass::hasComponents(const std::string &libName) {
  std::size_t foundBoost, foundQt;
  std::string stripPackage;
  std::string stripComponent;
  foundBoost = libName.find("boost");
  foundQt = libName.find("Qt");
  if (foundBoost != std::string::npos && foundBoost == 0) {
    stripComponent = libName.substr(6, libName.length());
    _boostComponents.push_back(stripComponent);
    return true;
  } else if (foundQt != std::string::npos && foundBoost == 0) {
    stripComponent = libName.substr(4, libName.length());
    this->_QtComponents.push_back(stripComponent);
    return true;
  } else {
    return false;
  }
}
std::string PrepClass::cmakeOutput() {
  for (std::vector<std::string>::iterator it = this->_fullLibNames.begin();
       it != this->_fullLibNames.end(); ++it) {
    std::string libName = stripLibName(*it);
    std::string module = libName;
    module[0] = toupper(module[0]);
    if (!hasComponents(libName)) {
      this->_cmakeOutput << "#Find " << libName << "\n"
                         << "find_library(" << libName << "_DBG NAMES "
                         << libName << " HINTS "
                         << "${DBG_LIB_PATH})\n"
                         << "find_library(" << libName << "_RLS NAMES "
                         << libName << " HINTS "
                         << "${RLS_LIB_PATH})\n"
                         << "set(dbgLIBS ${dbgLIBS} ${" << libName << "_DBG})\n"
                         << "set(rlsLIBS ${rlsLIBS} ${" << libName
                         << "_RLS})\n\n";
    }
  }
  if (this->_boostComponents.size() > 1) {
    this->_cmakeOutput << "#Find "
                       << "Boost components"
                       << "\n"
                       << "find_package(Boost COMPONENTS ";
    for (std::vector<std::string>::iterator it = this->_boostComponents.begin();
         it != this->_boostComponents.end(); ++it) {
      this->_cmakeOutput << *it << " ";
    }
    this->_cmakeOutput << "REQUIRED)\n"
                       << "set(dbgLIBS ${dbgLIBS} ${Boost_LIBRARIES})\n"
                       << "set(rlsLIBS ${rlsLIBS} ${Boost_LIBRARIES})\n\n";
  }
  if (this->_QtComponents.size() > 1) {
    this->_cmakeOutput << "#Find "
                       << "Qt components"
                       << "\n"
                       << "find_package(Qt5 COMPONENTS ";
    for (std::vector<std::string>::iterator jt = this->_QtComponents.begin();
         jt != this->_QtComponents.end(); ++jt) {
      this->_cmakeOutput << *jt << " ";
    }
    this->_cmakeOutput << "REQUIRED)\n"
                       << "set(dbgLIBS ${dbgLIBS} ${Qt5_LIBRARIES})\n"
                       << "set(rlsLIBS ${rlsLIBS} ${Qt5_LIBRARIES})\n\n";
  }
  return this->_cmakeOutput.str();
}

std::vector<std::string> &PrepClass::fullLibNames() {
  return this->_fullLibNames;
}

std::vector<std::string> &PrepClass::dbgLocalLibNames() {
  return this->_dbgLocalLibNames;
}

std::vector<std::string> &PrepClass::rlsLocalLibNames() {
  return this->_rlsLocalLibNames;
}

std::string PrepClass::findLib(const std::string &line) {
  std::string libName;
  size_t found;
  found = line.find("debug/lib/");
  if (found != std::string::npos) {
    libName = line.substr(found + 10, line.length());
  }
  return libName;
}
} // namespace vcbld
