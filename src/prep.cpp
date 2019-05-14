#include "prep.h"
#include "pch.h"

namespace vcbld {

PrepClass::PrepClass() {
  std::set<fs::directory_entry> dirEntry;
  std::string arch = "_" + architecture() + ".list";
  std::string temp, temp2;
  size_t foundFile;
  std::string listsPath =
      vcpkgDirPath() + "/" + "installed" + "/" + +"vcpkg" + "/" + "info";
  if (fs::is_directory(listsPath)) {
    std::copy(fs::directory_iterator(listsPath), fs::directory_iterator(),
              std::inserter(dirEntry, dirEntry.begin()));
    for (auto &it : dirEntry) {

      for (auto &jt : packageNames()) {
        foundFile = it.path().filename().string().find(jt);
        if (foundFile != std::string::npos && foundFile == 0) {
          std::string fileName = it.path().filename().string();
          std::string line;
          try {
            std::ifstream ifs(it.path());
            if (ifs.is_open()) {
              while (!ifs.eof()) {
                getline(ifs, line);
                if (!findDbgLib(line).empty()) {
                  _fullDbgLibNames.push_back(findDbgLib(line));
                }
                if (!findRlsLib(line).empty()) {
                  _fullLibNames.push_back(findRlsLib(line));
                }
                if (!findWinDbgDll(line).empty()) {
                  _windDbgDlls.push_back(findWinDbgDll(line));
                }
                if (!findWinRlsDll(line).empty()) {
                  _winRlsDlls.push_back(findWinRlsDll(line));
                }
              }
            }
          } catch (...) {
            // fail quietly!
          }
        }
      }
    }
  }

  if (!libDirectory().empty()) {
    std::string localDbgLibs = libDirectory() + "/" + "debug";
    std::set<fs::directory_entry> localDbgDirEntry;

    if (fs::is_directory(localDbgLibs)) {
      std::copy(fs::directory_iterator(localDbgLibs), fs::directory_iterator(),
                std::inserter(localDbgDirEntry, localDbgDirEntry.begin()));

      for (auto &it : localDbgDirEntry) {
        if ((it.path().filename().string()).at(0) != '.') {
          _dbgLocalLibNames.push_back(it.path().filename().string());
        }
      }
    }

    std::string localRlsLibs = libDirectory() + "/" + "release";
    std::vector<fs::directory_entry> localRlsDirEntry;

    if (fs::is_directory(localRlsLibs)) {
      std::copy(fs::directory_iterator(localRlsLibs), fs::directory_iterator(),
                std::inserter(localRlsDirEntry, localRlsDirEntry.begin()));

      for (auto &it : localRlsDirEntry) {
        if ((it.path().filename().string()).at(0) != '.') {
          _rlsLocalLibNames.push_back(it.path().filename().string());
        }
      }
    }
  }
}

std::string PrepClass::sourceFiles() {
  std::set<fs::directory_entry> dirEntry;
  std::string tempPath, fullPath;
  std::ostringstream temp;
  tempPath = sourceDirectory();
  if (fs::is_directory(tempPath)) {
    std::copy(fs::directory_iterator(tempPath), fs::directory_iterator(),
              std::inserter(dirEntry, dirEntry.begin()));

    _srcCount = dirEntry.size();

    for (auto &it : dirEntry) {
      if (fs::path(it.path().filename()).extension() == ".cpp" ||
          fs::path(it.path().filename()).extension() == ".rc" ||
          fs::path(it.path().filename()).extension() == ".c" ||
          fs::path(it.path().filename()).extension() == ".cxx" ||
          fs::path(it.path().filename()).extension() == ".qrc") {
        fullPath = std::move(it.path().string());
        posixify(fullPath);
        temp << "\"" << fullPath << "\" ";
      }
    }
  }
  return temp.str();
}

std::string PrepClass::sourceFilesSinPath() {
  std::set<fs::directory_entry> dirEntry;
  std::string tempPath;
  std::ostringstream temp;
  tempPath = sourceDirectory();
  if (fs::is_directory(tempPath)) {
    std::copy(fs::directory_iterator(tempPath), fs::directory_iterator(),
              std::inserter(dirEntry, dirEntry.begin()));

    for (auto &it : dirEntry) {
      if (fs::path(it.path().filename()).extension() == ".cpp" ||
          fs::path(it.path().filename()).extension() == ".rc" ||
          fs::path(it.path().filename()).extension() == ".c" ||
          fs::path(it.path().filename()).extension() == ".cxx" ||
          fs::path(it.path().filename()).extension() == ".qrc") {
        temp << it.path().filename().string() << " ";
      }
    }
  }
  return temp.str();
}

std::string PrepClass::headerPaths() {
  std::ostringstream temp;
  temp << " -I\"" << sourceDirectory() << "\" -I\"" << outputDirectory()
       << "\"";
  if (!includeDirectory().empty() && fs::exists(includeDirectory())) {
    temp << " -I\"" << includeDirectory() << "\"";
  }
  temp << " -I\"" << vcpkgDirPath() << "/"
       << "installed"
       << "/" << architecture() << "/"
       << "include\"";
  return temp.str();
}

std::string PrepClass::stripLibName(const std::string &lib) {
  std::size_t found, foundLib;
  std::string stripLib;
  std::string stripExt;
  foundLib = lib.find("lib");
  if (foundLib != std::string::npos && foundLib == 0) {
    stripLib = lib.substr(3, lib.length());
  } else {
    stripLib = lib;
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
  if (!libDirectory().empty() && fs::exists(libDirectory() + "/" + "debug")) {
    std::string localDbgLibs = libDirectory() + "/" + "debug";
    if (compilerPath().find("cl.exe") == std::string::npos) {
      temp << " -L\"" << localDbgLibs << "\" ";
    } else {
      temp << " -LIBPATH:\"" << localDbgLibs << "\" ";
    }
    for (auto &it : _dbgLocalLibNames) {
      if (compilerPath().find("cl.exe") == std::string::npos) {
        temp << "-l" << stripLibName(it) << " ";
      } else {
        temp << " " << it << " ";
      }
    }
  }
  std::string dbgLibPath = vcpkgDirPath() + "/" + "installed" + "/" +
                           architecture() + "/" + "debug" + "/" + "lib";
  if (compilerPath().find("cl.exe") == std::string::npos) {
    temp << " -L\"" << dbgLibPath << "\" ";
  } else {
    temp << " -LIBPATH:\"" << dbgLibPath << "\" ";
  }
  for (auto &it : _fullDbgLibNames) {
    if (compilerPath().find("cl.exe") == std::string::npos) {
      temp << "-l" << stripLibName(it) << " ";
    } else {
      temp << " " << it << " ";
    }
  }
  return temp.str();
}

std::string PrepClass::rlsLibPaths() {
  std::ostringstream temp;
  if (!libDirectory().empty() && fs::exists(libDirectory() + "/release")) {
    std::string localRlsLibs = libDirectory() + "/" + "release";
    if (compilerPath().find("cl.exe") == std::string::npos) {
      temp << " -L\"" << localRlsLibs << "\" ";
    } else {
      temp << " -LIBPATH:\"" << localRlsLibs << "\" ";
    }
    for (auto &it : _rlsLocalLibNames) {
      if (compilerPath().find("cl.exe") == std::string::npos) {
        temp << "-l" << stripLibName(it) << " ";
      } else {
        temp << " " << it << " ";
      }
    }
  }
  std::string rlsLibPath =
      vcpkgDirPath() + "/" + "installed" + "/" + architecture() + "/" + "lib";
  if (compilerPath().find("cl.exe") == std::string::npos) {
    temp << " -L\"" << rlsLibPath << "\" ";
  } else {
    temp << " -LIBPATH:\"" << rlsLibPath << "\" ";
  }
  for (auto &it : _fullLibNames) {
    if (compilerPath().find("cl.exe") == std::string::npos) {
      temp << "-l" << stripLibName(it) << " ";
    } else {
      temp << " " << it << " ";
    }
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
    _QtComponents.push_back(stripComponent);
    return true;
  } else {
    return false;
  }
}
std::string PrepClass::cmakeOutput() {

  for (auto &it : _dbgLocalLibNames) {
    std::string libName;
    if (compilerPath().find("cl.exe") != std::string::npos) {
      libName = it;
    } else {
      libName = stripLibName(it);
    }

    _cmakeOutput << "find_library(" << libName << "_DBG NAMES " << libName
                 << " HINTS "
                 << "${LOCAL_DBG_LIB_PATH})\n"
                 << "set(dbgLIBS ${dbgLIBS} ${" << libName << "_DBG})\n\n";
  }

  for (auto &it : _rlsLocalLibNames) {
    std::string libName;
    if (compilerPath().find("cl.exe") != std::string::npos) {
      libName = it;
    } else {
      libName = stripLibName(it);
    }
    _cmakeOutput << "find_library(" << libName << "_RLS NAMES " << libName
                 << " HINTS "
                 << "${LOCAL_RLS_LIB_PATH})\n"
                 << "set(rlsLIBS ${rlsLIBS} ${" << libName << "_RLS})\n\n";
  }

  for (auto it = _fullLibNames.begin(); it != _fullLibNames.end(); ++it) {
    std::string libName;
    if (compilerPath().find("cl.exe") != std::string::npos) {
      libName = *it;
    } else {
      libName = stripLibName(*it);
    }
    std::string module = libName;
    module[0] = toupper(module[0]);
    if (!hasComponents(libName)) {
      _cmakeOutput << "#Find " << libName << "\n"
                   << "find_library(" << libName << "_DBG NAMES ";
      if (compilerPath().find("cl.exe") != std::string::npos) {
        _cmakeOutput
            << _fullDbgLibNames[std::distance(_fullLibNames.begin(), it)]
            << " HINTS ";
      } else {
        _cmakeOutput
            << stripLibName(
                   _fullDbgLibNames[std::distance(_fullLibNames.begin(), it)])
            << " HINTS ";
      }
      _cmakeOutput << "${VCPKG_DBG_LIB_PATH})\n"
                   << "find_library(" << libName << "_RLS NAMES " << libName
                   << " HINTS "
                   << "${VCPKG_RLS_LIB_PATH})\n"
                   << "set(dbgLIBS ${dbgLIBS} ${" << libName << "_DBG})\n"
                   << "set(rlsLIBS ${rlsLIBS} ${" << libName << "_RLS})\n\n";
    }
  }
  if (_boostComponents.size() > 1) {
    _cmakeOutput << "#Find "
                 << "Boost components"
                 << "\n"
                 << "find_package(Boost COMPONENTS system ";
    for (auto &it : _boostComponents) {
      _cmakeOutput << it << " ";
    }
    _cmakeOutput << "REQUIRED)\n"
                 << "set(dbgLIBS ${dbgLIBS} ${Boost_LIBRARIES})\n"
                 << "set(rlsLIBS ${rlsLIBS} ${Boost_LIBRARIES})\n\n";
  }
  if (_QtComponents.size() > 1) {
    _cmakeOutput << "#Find "
                 << "Qt components"
                 << "\n"
                 << "find_package(Qt5 COMPONENTS ";
    for (auto &it : _QtComponents) {
      _cmakeOutput << it << " ";
    }
    _cmakeOutput << "REQUIRED)\n"
                 << "set(dbgLIBS ${dbgLIBS} ${Qt5_LIBRARIES})\n"
                 << "set(rlsLIBS ${rlsLIBS} ${Qt5_LIBRARIES})\n\n";
  }
  return _cmakeOutput.str();
}

std::vector<std::string> &PrepClass::fullLibNames() { return _fullLibNames; }

std::vector<std::string> &PrepClass::fullDbgLibNames() {
  return _fullDbgLibNames;
}

std::vector<std::string> &PrepClass::dbgLocalLibNames() {
  return _dbgLocalLibNames;
}

std::vector<std::string> &PrepClass::rlsLocalLibNames() {
  return _rlsLocalLibNames;
}

std::vector<std::string> &PrepClass::winDbgDlls() { return _windDbgDlls; }

std::vector<std::string> &PrepClass::winRlsDlls() { return _winRlsDlls; }

std::string PrepClass::findDbgLib(const std::string &line) {
  std::string libName;
  size_t found;
  found = line.find("debug/lib/");
  if (found != std::string::npos) {
    libName = line.substr(found + 10, line.length());
  }
  if (libName.find(".a") != std::string::npos ||
      libName.find(".lib") != std::string::npos ||
      libName.find(".so") != std::string::npos ||
      libName.find(".dylib") != std::string::npos) {
    return libName;
  } else {
    return "";
  }
}

std::string PrepClass::findRlsLib(const std::string &line) {
  std::string libName, toFind;
  toFind = architecture() + "/lib/";
  size_t found;
  found = line.find(toFind);
  if (found != std::string::npos) {
    libName = line.substr(found + toFind.length(), line.length());
  }
  if (libName.find(".a") != std::string::npos ||
      libName.find(".lib") != std::string::npos ||
      libName.find(".so") != std::string::npos ||
      libName.find(".dylib") != std::string::npos) {
    return libName;
  } else {
    return "";
  }
}

std::string PrepClass::findWinDbgDll(const std::string &line) {
  std::string dllPath;
  size_t found;
  found = line.find("debug/bin/");
  if (found != std::string::npos && line.find(".dll") != std::string::npos) {
    dllPath = line.substr(0, line.length());
  }
  return dllPath;
}

std::string PrepClass::findWinRlsDll(const std::string &line) {
  std::string dllPath;
  size_t found;
  found = line.find(architecture() + "/bin/");
  if (found != std::string::npos && line.find(".dll") != std::string::npos) {
    dllPath = line.substr(0, line.length());
  }
  return dllPath;
}

void PrepClass::posixify(std::string &path) {
  if (path.find('\\') != std::string::npos) {
    path.replace(path.find('\\'), 1, "/");
    posixify(path);
  }
}

size_t PrepClass::srcCount() const { return _srcCount; }

} // namespace vcbld
