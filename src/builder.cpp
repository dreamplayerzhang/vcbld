#include "pch.h"

namespace vcbld {

Builder::Builder(const std::string &buildType)
    : PrepClass(), _buildType(buildType) {
  if (!fs::exists("vcbld.json")) {
    std::cout << "Build configuration not found!" << std::endl;
    std::exit(1);
  }

  _dbgDir = outputDirectory() + "/" + "debug";
  _rlsDir = outputDirectory() + "/" + "release";

  if (_buildType == "debug") {
    _buildDir = _dbgDir;
  } else {
    _buildDir = _rlsDir;
  }
  compile();
  appLink();
  dylibLink();
  archive();
}

void Builder::compile() {
  std::string debugFlag, keepFlag, stdFlag;
  if (compilerPath().find("cl.exe") != std::string::npos) {
    debugFlag = " -DEBUG ";
    keepFlag = " -c -EHsc ";
    stdFlag = "/std:";
  } else {
    debugFlag = " -g ";
    keepFlag = " -c ";
    stdFlag = "-std=";
  }

  if (_buildType == "release") {
    _compileCommand << "\"" << compilerPath() << "\" " << headerPaths() << " "
                    << compilerFlags() << " " << compilerDefines() << " "
                    << stdFlag << language() << standard() << keepFlag
                    << sourceFiles();
  } else {
    _compileCommand << "\"" << compilerPath() << "\" " << headerPaths() << " "
                    << compilerFlags() << " " << compilerDefines() << debugFlag
                    << stdFlag << language() << standard() << keepFlag
                    << sourceFiles();
  }
}

void Builder::appLink() {
  std::string linker, outFlag, ext;
  std::string debugFlag, keepFlag, stdFlag;
  if (compilerPath().find("cl.exe") != std::string::npos) {
    linker =
        (fs::canonical(compilerPath()).parent_path() / "link.exe ").string();
    outFlag = " -OUT:";
    ext = ".exe";
  } else {
    linker = compilerPath();
    outFlag = " -o ";
  }

  if (_buildType == "release") {
    _appLinkCmnd << "\"" << linker << "\"" << outFlag << binaryName() << ext
                 << " " << objPath(_buildDir) << " " << rlsLibPaths() << " "
                 << linkerFlags();
  } else {
    _appLinkCmnd << "\"" << linker << "\"" << outFlag << binaryName() << ext
                 << " " << objPath(_buildDir) << " " << dbgLibPaths() << " "
                 << linkerFlags();
  }
}

void Builder::dylibLink() {
  std::string linker, outFlag, dylibArg, dylibExt;
  if (compilerPath().find("clang") != std::string::npos) {
    linker = compilerPath();
    outFlag = " -o ";
    dylibArg = " -dynamiclib ";
    dylibExt = ".dylib";
  } else if (compilerPath().find("cl.exe") != std::string::npos) {
    linker =
        (fs::canonical(compilerPath()).parent_path() / "link.exe ").string();
    outFlag = " -OUT:";
    dylibArg = " -DLL ";
    dylibExt = ".dll";
  } else {
    linker = compilerPath();
    outFlag = " -o ";
    dylibArg = " -shared ";
    dylibExt = ".so";
  }

  if (_buildType == "release") {
    _libLinkCmnd << "\"" << linker << "\"" << dylibArg << outFlag
                 << binaryName() << dylibExt << " " << objPath(_buildDir) << " "
                 << rlsLibPaths() << " " << linkerFlags();
  } else {
    _libLinkCmnd << "\"" << linker << "\"" << dylibArg << outFlag
                 << binaryName() << dylibExt << " " << objPath(_buildDir) << " "
                 << dbgLibPaths() << " " << linkerFlags();
  }
}

void Builder::archive() {
  std::string archiver, ext;
  if (compilerPath().find("cl.exe") != std::string::npos) {
    archiver =
        (fs::canonical(compilerPath()).parent_path() / "lib.exe ").string() +
        "\" -OUT:";
    ext = ".lib";
  } else {
    archiver = archiverPath() + "\" rcs ";
    ext = ".a";
  }
  if (_buildType == "release") {
    _archiveCmnd << "\"" << archiverPath() << "\" rcs " << binaryName() << ext
                 << " " << objPath(_buildDir);
  } else {
    _archiveCmnd << "\"" << archiverPath() << "\" rcs " << binaryName() << ext
                 << " " << objPath(_buildDir);
  }
}

std::string Builder::getBldCommands() {
  if (binaryType() == "app") {
    return _compileCommand.str() + "\n" + _appLinkCmnd.str();
  } else if (binaryType() == "staticLibrary") {
    return _compileCommand.str() + "\n" + _archiveCmnd.str();
  } else if (binaryType() == "dynamicLibrary") {
    return _compileCommand.str() + "\n" + _libLinkCmnd.str();
  } else {
    std::cout << "Unknown binary type defined in vcbld.json." << std::endl;
    return "";
  }
}

void Builder::build() {
  std::string buildDir;
  if (_buildType == "debug") {
    buildDir = _dbgDir;
  } else {
    buildDir = _rlsDir;
  }
  if (!fs::exists(outputDirectory()))
    fs::create_directory(outputDirectory());
  if (!fs::exists(outputDirectory() + "/" + "release"))
    fs::create_directory(_rlsDir);
  if (!fs::exists(outputDirectory() + "/" + "debug"))
    fs::create_directory(_dbgDir);

  if (binaryType() == "app") {
    try {
      std::cout << "Compiling in " << _buildType << "...\n";
      exec("cd \"" + buildDir + "\" && " + compileCommand());
    } catch (const std::exception &e) {
      std::cout << "Compilation failed!" << std::endl;
      std::cerr << e.what() << " " << errno << std::endl;
    }
    try {
      std::cout << "Linking...\n";
      exec("cd \"" + buildDir + "\" && " + appLinkCmnd());
      std::cout << "Done\n";
    } catch (const std::exception &e) {
      std::cout << "Linking failed!" << std::endl;
      std::cerr << e.what() << " " << errno << std::endl;
    }
    try {
      copy();
    } catch (const std::exception &e) {
      std::cout << "Libraries exist in output directory." << std::endl;
      std::cerr << e.what() << " " << errno << std::endl;
    }
  } else if (binaryType() == "staticLibrary") {
    try {
      std::cout << "Compiling in " << _buildType << "...\n";
      exec("cd \"" + buildDir + "\" && " + compileCommand());
    } catch (const std::exception &e) {
      std::cout << "Compilation failed!" << std::endl;
      std::cerr << e.what() << " " << errno << std::endl;
    }
    try {
      std::cout << "Archiving...\n";
      exec("cd \"" + buildDir + "\" && " + archiveCmnd());
      std::cout << "Done\n";
    } catch (const std::exception &e) {
      std::cout << "Archiving failed!" << std::endl;
      std::cerr << e.what() << errno << std::endl;
    }
  } else if (binaryType() == "dynamicLibrary") {
    try {
      std::cout << "Compiling in " << _buildType << "...\n";
      exec("cd \"" + buildDir + "\" && " + compileCommand());
    } catch (const std::exception &e) {
      std::cout << "Compilation failed!" << std::endl;
      std::cerr << e.what() << " " << errno << std::endl;
    }
    try {
      std::cout << "Linking...\n";
      exec("cd \"" + buildDir + "\" && " + libLinkCmnd());
      std::cout << "Done\n";
    } catch (const std::exception &e) {
      std::cout << "Linking failed!" << std::endl;
      std::cerr << e.what() << errno << std::endl;
    }
    try {
      copy();
    } catch (const std::exception &e) {
      std::cout << "Libraries exist in output directory." << std::endl;
      std::cerr << e.what() << " " << errno << std::endl;
    }
  } else {
    std::cout << "Unknown binary type defined in vcbld.json." << std::endl;
  }
}

void Builder::copy() {
  std::string dbgLibPath = vcpkgDirPath() + "/" + "installed" + "/" +
                           architecture() + "/" + "debug/lib";
  std::string rlsLibPath =
      vcpkgDirPath() + "/" + "installed" + "/" + architecture() + "/" + "lib";
  std::string fullName;
  if (libDirectory() != "") {
    std::string localDbgPath = libDirectory() + "/" + "debug";
    std::string localRlsPath = libDirectory() + "/" + "release";
    if (_buildType == "debug") {
      for (std::vector<std::string>::iterator it = dbgLocalLibNames().begin();
           it != dbgLocalLibNames().end(); ++it) {
        fullName = localDbgPath + "/" + (*it);
        if (fs::exists(fullName)) {
          if (it->find(".a") == std::string::npos &&
              it->find(".lib") == std::string::npos) {
            if (!fs::exists(_dbgDir + "/" + (*it))) {
              fs::copy(fullName, _dbgDir);
            }
          }
        }
      }
    } else {
      for (std::vector<std::string>::iterator it = rlsLocalLibNames().begin();
           it != rlsLocalLibNames().end(); ++it) {
        fullName = localRlsPath + "/" + (*it);
        if (fs::exists(fullName)) {
          if (it->find(".a") == std::string::npos &&
              it->find(".lib") == std::string::npos) {
            if (!fs::exists(_rlsDir + "/" + (*it))) {
              fs::copy(fullName, _rlsDir);
            }
          }
        }
      }
    }
  }
  for (std::vector<std::string>::iterator it = fullLibNames().begin();
       it != fullLibNames().end(); ++it) {
    if (_buildType == "debug") {
      fullName = dbgLibPath + "/" +
                 fullDbgLibNames()[std::distance(fullLibNames().begin(), it)];
    } else {
      fullName = rlsLibPath + "/" + (*it);
    }
    if (fs::exists(fullName)) {
      if (_buildType == "debug") {
        if (it->find(".a") == std::string::npos &&
            it->find(".lib") == std::string::npos) {
          if (!fs::exists(_dbgDir + "/" + (*it))) {
            fs::copy(fullName, _dbgDir);
          }
        }
      } else {
        if (it->find(".a") == std::string::npos &&
            it->find(".lib") == std::string::npos) {
          if (!fs::exists(_rlsDir + "/" + (*it))) {
            fs::copy(fullName, _rlsDir);
          }
        }
      }
    }
  }
}

std::string Builder::compileCommand() const { return _compileCommand.str(); }
std::string Builder::appLinkCmnd() const { return _appLinkCmnd.str(); }

std::string Builder::libLinkCmnd() const { return _libLinkCmnd.str(); }

std::string Builder::archiveCmnd() const { return _archiveCmnd.str(); }

void Builder::exec(const std::string &command) {
  try {
    int systemRet = system(command.c_str());
    if (systemRet == -1) {
      std::cout << "An error occured." << std::endl;
    }
  } catch (const std::exception &e) {
    std::cout << "An error occured." << std::endl;
    std::cerr << e.what() << std::endl;
  }
}
} // namespace vcbld
