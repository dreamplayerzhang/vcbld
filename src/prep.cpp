#include "pch.h"

namespace vcbld {

PrepClass::PrepClass() {
  std::vector<fs::directory_entry> dirEntry;
  std::string arch = "_" + architecture() + ".list";
  std::string temp, temp2;
  size_t foundFile;
  std::string listsPath =
      vcpkgDirPath() + "/" + "installed" + "/" + +"vcpkg" + "/" + "info";
  if (fs::is_directory(static_cast<fs::path>(listsPath))) {
    std::copy(fs::directory_iterator(listsPath), fs::directory_iterator(),
              back_inserter(dirEntry));
    std::sort(dirEntry.begin(), dirEntry.end());
    dirEntry.erase(std::unique(dirEntry.begin(), dirEntry.end()),
                   dirEntry.end());
    for (std::vector<fs::directory_entry>::iterator it = dirEntry.begin();
         it != dirEntry.end(); ++it) {

      for (std::vector<std::string>::iterator jt = packageNames().begin();
           jt != packageNames().end(); ++jt) {
        foundFile = it->path().filename().string().find(*jt);
        if (foundFile != std::string::npos && foundFile == 0) {
          std::string fileName = it->path().filename().string();
          std::string line;
          try {
            std::ifstream ifs(it->path());
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
        std::sort(_fullDbgLibNames.begin(), _fullDbgLibNames.end());
        _fullDbgLibNames.erase(
            std::unique(_fullDbgLibNames.begin(), _fullDbgLibNames.end()),
            _fullDbgLibNames.end());

        std::sort(_fullLibNames.begin(), _fullLibNames.end());
        _fullLibNames.erase(
            std::unique(_fullLibNames.begin(), _fullLibNames.end()),
            _fullLibNames.end());

        std::sort(_windDbgDlls.begin(), _windDbgDlls.end());
        _windDbgDlls.erase(
            std::unique(_windDbgDlls.begin(), _windDbgDlls.end()),
            _windDbgDlls.end());

        std::sort(_winRlsDlls.begin(), _winRlsDlls.end());
        _winRlsDlls.erase(std::unique(_winRlsDlls.begin(), _winRlsDlls.end()),
                          _winRlsDlls.end());
      }
    }
  }

  if (!libDirectory().empty()) {
    std::string localDbgLibs = libDirectory() + "/" + "debug";
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
        if ((jt->path().filename().string()).at(0) != '.') {
          _dbgLocalLibNames.push_back(jt->path().filename().string());
        }
      }
    }

    std::string localRlsLibs = libDirectory() + "/" + "release";
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
        if ((jt->path().filename().string()).at(0) != '.') {
          _rlsLocalLibNames.push_back(jt->path().filename().string());
        }
      }
    }
  }
}

std::string PrepClass::sourceFiles() {
  std::vector<fs::directory_entry> dirEntry;
  std::string tempPath, fullPath;
  std::ostringstream temp;
  tempPath = sourceDirectory();
  if (fs::is_directory(static_cast<fs::path>(tempPath))) {
    std::copy(fs::directory_iterator(tempPath), fs::directory_iterator(),
              back_inserter(dirEntry));
    std::sort(dirEntry.begin(), dirEntry.end());
    dirEntry.erase(std::unique(dirEntry.begin(), dirEntry.end()),
                   dirEntry.end());
    _srcCount = dirEntry.size();

    for (std::vector<fs::directory_entry>::iterator it = dirEntry.begin();
         it != dirEntry.end(); ++it) {
      if (fs::path(it->path().filename()).extension() == ".cpp" ||
          fs::path(it->path().filename()).extension() == ".rc" ||
          fs::path(it->path().filename()).extension() == ".c" ||
          fs::path(it->path().filename()).extension() == ".cxx" ||
          fs::path(it->path().filename()).extension() == ".qrc" ||
          fs::path(it->path().filename()).extension() == ".uic" ||
          fs::path(it->path().filename()).extension() == ".ui") {
        fullPath = std::move(it->path().string());
        posixify(fullPath);
        temp << "\"" << fullPath << "\" ";
      }
    }
  }
  return temp.str();
}

std::string PrepClass::sourceFilesSinPath() {
  std::vector<fs::directory_entry> dirEntry;
  std::string tempPath;
  std::ostringstream temp;
  tempPath = sourceDirectory();
  if (fs::is_directory(static_cast<fs::path>(tempPath))) {
    std::copy(fs::directory_iterator(tempPath), fs::directory_iterator(),
              back_inserter(dirEntry));
    std::sort(dirEntry.begin(), dirEntry.end());
    dirEntry.erase(std::unique(dirEntry.begin(), dirEntry.end()),
                   dirEntry.end());

    for (std::vector<fs::directory_entry>::iterator it = dirEntry.begin();
         it != dirEntry.end(); ++it) {
      if (fs::path(it->path().filename()).extension() == ".cpp" ||
          fs::path(it->path().filename()).extension() == ".rc" ||
          fs::path(it->path().filename()).extension() == ".c" ||
          fs::path(it->path().filename()).extension() == ".cxx" ||
          fs::path(it->path().filename()).extension() == ".qrc" ||
          fs::path(it->path().filename()).extension() == ".uic" ||
          fs::path(it->path().filename()).extension() == ".ui") {
        temp << it->path().filename().string() << " ";
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
    for (std::vector<std::string>::iterator jt = _dbgLocalLibNames.begin();
         jt != _dbgLocalLibNames.end(); ++jt) {
      if (compilerPath().find("cl.exe") == std::string::npos) {
        temp << " -L\"" << localDbgLibs << "\" "
             << "-l" << stripLibName(*jt);
      } else {
        temp << " -LIBPATH:\"" << localDbgLibs << "\" "
             << " " << *jt;
      }
    }
  }
  std::string dbgLibPath = vcpkgDirPath() + "/" + "installed" + "/" +
                           architecture() + "/" + "debug" + "/" + "lib";
  for (std::vector<std::string>::iterator it = _fullDbgLibNames.begin();
       it != _fullDbgLibNames.end(); ++it) {
    if (compilerPath().find("cl.exe") == std::string::npos) {
      temp << " -L\"" << dbgLibPath << "\" "
           << "-l" << stripLibName(*it);
    } else {
      temp << " -LIBPATH:\"" << dbgLibPath << "\" "
           << " " << *it;
    }
  }
  return temp.str();
}

std::string PrepClass::rlsLibPaths() {
  std::ostringstream temp;
  if (!libDirectory().empty() && fs::exists(libDirectory() + "/release")) {
    std::string localRlsLibs = libDirectory() + "/" + "release";
    for (std::vector<std::string>::iterator jt = _rlsLocalLibNames.begin();
         jt != _rlsLocalLibNames.end(); ++jt) {
      if (compilerPath().find("cl.exe") == std::string::npos) {
        temp << " -L\"" << localRlsLibs << "\" "
             << "-l" << stripLibName(*jt);
      } else {
        temp << " -LIBPATH:\"" << localRlsLibs << "\" "
             << " " << *jt;
      }
    }
  }
  std::string rlsLibPath =
      vcpkgDirPath() + "/" + "installed" + "/" + architecture() + "/" + "lib";
  for (std::vector<std::string>::iterator it = _fullLibNames.begin();
       it != _fullLibNames.end(); ++it) {
    if (compilerPath().find("cl.exe") == std::string::npos) {
      temp << " -L\"" << rlsLibPath << "\" "
           << "-l" << stripLibName(*it);
    } else {
      temp << " -LIBPATH:\"" << rlsLibPath << "\" "
           << " " << *it;
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

  for (std::vector<std::string>::iterator it = _dbgLocalLibNames.begin();
       it != _dbgLocalLibNames.end(); ++it) {
    std::string libName;
    if (compilerPath().find("cl.exe") != std::string::npos) {
      libName = *it;
    } else {
      libName = stripLibName(*it);
    }

    _cmakeOutput << "find_library(" << libName << "_DBG NAMES " << libName
                 << " HINTS "
                 << "${LOCAL_DBG_LIB_PATH})\n"
                 << "set(dbgLIBS ${dbgLIBS} ${" << libName << "_DBG})\n\n";
  }

  for (std::vector<std::string>::iterator it = _rlsLocalLibNames.begin();
       it != _rlsLocalLibNames.end(); ++it) {
    std::string libName;
    if (compilerPath().find("cl.exe") != std::string::npos) {
      libName = *it;
    } else {
      libName = stripLibName(*it);
    }
    _cmakeOutput << "find_library(" << libName << "_RLS NAMES " << libName
                 << " HINTS "
                 << "${LOCAL_RLS_LIB_PATH})\n"
                 << "set(rlsLIBS ${rlsLIBS} ${" << libName << "_RLS})\n\n";
  }

  for (std::vector<std::string>::iterator it = _fullLibNames.begin();
       it != _fullLibNames.end(); ++it) {
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
      _cmakeOutput
          << stripLibName(
                 _fullDbgLibNames[std::distance(_fullLibNames.begin(), it)])
          << " HINTS "
          << "${VCPKG_DBG_LIB_PATH})\n"
          << "find_library(" << libName << "_RLS NAMES " << libName << " HINTS "
          << "${VCPKG_RLS_LIB_PATH})\n"
          << "set(dbgLIBS ${dbgLIBS} ${" << libName << "_DBG})\n"
          << "set(rlsLIBS ${rlsLIBS} ${" << libName << "_RLS})\n\n";
    }
  }
  if (_boostComponents.size() > 1) {
    _cmakeOutput << "#Find "
                 << "Boost components"
                 << "\n"
                 << "find_package(Boost COMPONENTS ";
    for (std::vector<std::string>::iterator it = _boostComponents.begin();
         it != _boostComponents.end(); ++it) {
      _cmakeOutput << *it << " ";
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
    for (std::vector<std::string>::iterator jt = _QtComponents.begin();
         jt != _QtComponents.end(); ++jt) {
      _cmakeOutput << *jt << " ";
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
  return libName;
}

std::string PrepClass::findRlsLib(const std::string &line) {
  std::string libName, toFind;
  toFind = architecture() + "/lib/";
  size_t found;
  found = line.find(toFind);
  if (found != std::string::npos) {
    libName = line.substr(found + toFind.length(), line.length());
  }
  return libName;
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
