#include "builder.h"

#include <algorithm>
#include <errno.h>
#if defined(_WIN32) || defined(_WIN64)
#include <filesystem>
#else
#include <experimental/filesystem>
#endif
#include <iostream>
#include <iterator>
#include <vector>

namespace fs = std::experimental::filesystem;

namespace vcbld {

Builder::Builder(const std::string &buildType)
    : _buildType(buildType), PrepClass() {
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
}

void Builder::compile() {

  if (_buildType == "release") {
    _compileCommand << "cd " << _rlsDir << " && \""
                          << compilerPath() << "\" "
                          << headerPaths() << " " << compilerFlags()
                          << " " << compilerDefines() << " "
                          << "-std=" << language() << standard()
                          << " -c " << sourceFiles();
  } else {
    _compileCommand << "cd " << _dbgDir << " && \""
                          << compilerPath() << "\" "
                          << headerPaths() << " " << compilerFlags()
                          << " " << compilerDefines() << " -g "
                          << "-std=" << language() << standard()
                          << " -c " << sourceFiles();
  }
  int systemRet = system(_compileCommand.str().c_str());
  if (systemRet == -1) {
    std::cout << "An error occured while compiling." << std::endl;
  }
}

void Builder::appLink() {
  if (_buildType == "release") {
    _appLinkCmnd << "cd " << _rlsDir << " && \""
                       << compilerPath() << "\" -o " << binaryName()
                       << " " << objPath(_buildDir) << " "
                       << rlsLibPaths() << " " << linkerFlags();
  } else {
    _appLinkCmnd << "cd " << _dbgDir << " && \""
                       << compilerPath() << "\" -o " << binaryName()
                       << " " << objPath(_buildDir) << " "
                       << dbgLibPaths() << " " << linkerFlags();
  }
  int systemRet = system(_appLinkCmnd.str().c_str());
  if (systemRet == -1) {
    std::cout << "An error occured while linking." << std::endl;
  }
}

void Builder::dylibLink() {
  std::string dylibArg, dylibExt;
  if (compilerPath().find("clang") != std::string::npos) {
    dylibArg = " -dynamiclib ";
    dylibExt = ".dylib";
  } else {
    dylibArg = " -shared ";
    dylibExt = ".so";
  }

  if (_buildType == "release") {
    _libLinkCmnd << "cd " << _rlsDir << " && \""
                       << compilerPath() << "\"" << dylibArg << " -o "
                       << binaryName() << dylibExt << " "
                       << objPath(_buildDir) << " "
                       << rlsLibPaths() << " " << linkerFlags();
  } else {
    _libLinkCmnd << "cd " << _dbgDir << " && \""
                       << compilerPath() << "\"" << dylibArg << " -o "
                       << binaryName() << dylibExt << " "
                       << objPath(_buildDir) << " "
                       << dbgLibPaths() << " " << linkerFlags();
  }
  int systemRet = system(_libLinkCmnd.str().c_str());
  if (systemRet == -1) {
    std::cout << "An error occured while linking." << std::endl;
  }
}

void Builder::archive() {
  if (_buildType == "release") {
    _archiveCmnd << "cd " << _rlsDir << " && \""
                       << archiverPath() << "\" rcs "
                       << binaryName() << ".a"
                       << " " << objPath(_buildDir);
  } else {
    _archiveCmnd << "cd " << _dbgDir << " && \""
                       << archiverPath() << "\" rcs "
                       << binaryName() << ".a"
                       << " " << objPath(_buildDir);
  }
  int systemRet = system(_archiveCmnd.str().c_str());
  if (systemRet == -1) {
    std::cout << "An error occured while archiving." << std::endl;
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

  if (!fs::exists(outputDirectory()))
    fs::create_directory(outputDirectory());
  if (!fs::exists(outputDirectory() + "/" + "release"))
    fs::create_directory(_rlsDir);
  if (!fs::exists(outputDirectory() + "/" + "debug"))
    fs::create_directory(_dbgDir);

  if (binaryType() == "app") {
    try {
      std::cout << "Compiling in " << _buildType << "...\n";
      compile();
    } catch (const std::exception &e) {
      std::cout << "Compilation failed!" << std::endl;
      std::cerr << e.what() << " " << errno << std::endl;
    }
    try {
      std::cout << "Linking...\n";
      appLink();
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
      compile();
    } catch (const std::exception &e) {
      std::cout << "Compilation failed!" << std::endl;
      std::cerr << e.what() << " " << errno << std::endl;
    }
    try {
      std::cout << "Archiving...\n";
      archive();
      std::cout << "Done\n";
    } catch (const std::exception &e) {
      std::cout << "Archiving failed!" << std::endl;
      std::cerr << e.what() << errno << std::endl;
    }
  } else if (binaryType() == "dynamicLibrary") {
    try {
      std::cout << "Compiling in " << _buildType << "...\n";
      compile();
    } catch (const std::exception &e) {
      std::cout << "Compilation failed!" << std::endl;
      std::cerr << e.what() << " " << errno << std::endl;
    }
    try {
      std::cout << "Linking...\n";
      dylibLink();
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
  std::string rlsLibPath = vcpkgDirPath() + "/" + "installed" + "/" +
                           architecture() + "/" + "lib";
  std::string fullName;
  if (libDirectory() != "") {
    std::string localDbgPath = libDirectory() + "/" + "debug";
    std::string localRlsPath = libDirectory() + "/" + "release";
    if (_buildType == "debug") {
      for (std::vector<std::string>::iterator it =
               dbgLocalLibNames().begin();
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
      for (std::vector<std::string>::iterator it =
               rlsLocalLibNames().begin();
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
      fullName = dbgLibPath + "/" + (*it);
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
} // namespace vcbld
