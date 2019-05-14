#include "init.h"
#include "conf.h"
#include "pch.h"

namespace vcbld {

Init::Init(const fs::path &vcbldPath) : _vcbldPath(vcbldPath) {
  std::string PATH = std::getenv("PATH");
  std::string brewLLVM = "/usr/local/opt/llvm/bin";
  std::string localBin = "/usr/local/bin";

  findPathDirs(PATH, _paths);

  if (fs::exists(brewLLVM)) {
    _paths.emplace_back(brewLLVM);
  }

  if (fs::exists(localBin)) {
    _paths.emplace_back(localBin);
  }

  if (fs::exists(_vcbldPath)) {
    _paths.emplace_back(fs::canonical(_vcbldPath));
    _paths.emplace_back(
        findCmake(fs::canonical(_vcbldPath) / "downloads" / "tools"));
  }

  if (fs::exists(_vcbldPath / "vcpkg") ||
      fs::exists(_vcbldPath / "vcpkg.exe")) {
    _paths.emplace_back(fs::canonical(_vcbldPath));
    _paths.emplace_back(
        findCmake(fs::canonical(_vcbldPath) / "downloads" / "tools"));
  }

  if (fs::exists(_vcbldPath.parent_path() / "vcpkg" / "vcpkg") ||
      fs::exists(_vcbldPath.parent_path() / "vcpkg" / "vcpkg.exe")) {
    _paths.emplace_back(fs::canonical(_vcbldPath).parent_path() / "vcpkg");
    _paths.emplace_back(findCmake(fs::canonical(_vcbldPath).parent_path() /
                                  "vcpkg" / "downloads" / "tools"));
  }

  if (fs::exists(_vcbldPath.parent_path().parent_path() / "vcpkg" / "vcpkg") ||
      fs::exists(_vcbldPath.parent_path().parent_path() / "vcpkg" /
                 "vcpkg.exe")) {
    _paths.emplace_back(fs::canonical(_vcbldPath).parent_path().parent_path() /
                        "vcpkg");
    _paths.emplace_back(
        findCmake(fs::canonical(_vcbldPath).parent_path().parent_path() /
                  "vcpkg" / "downloads" / "tools"));
  }

  if (fs::exists(_vcbldPath.parent_path().parent_path().parent_path() /
                 "vcpkg" / "vcpkg") ||
      fs::exists(_vcbldPath.parent_path().parent_path().parent_path() /
                 "vcpkg" / "vcpkg.exe")) {
    _paths.emplace_back(
        fs::canonical(_vcbldPath).parent_path().parent_path().parent_path() /
        "vcpkg");
    _paths.emplace_back(findCmake(
        fs::canonical(_vcbldPath).parent_path().parent_path().parent_path() /
        "vcpkg" / "downloads" / "tools"));
  }

  finder(_paths, "C:/");
  if (std::getenv("HOME") != nullptr)
    finder(_paths, std::getenv("HOME"));
  if (std::getenv("PROGRAMFILES") != nullptr)
    finder(_paths, std::getenv("PROGRAMFILES"));
  if (std::getenv("MINGW_HOME") != nullptr)
    finder(_paths, std::getenv("MINGW_HOME"));
  if (std::getenv("EMSCRIPTEN") != nullptr) {
    _paths.emplace_back(fs::canonical(std::getenv("EMSCRIPTEN")));
  }

  sorter(_paths);

  for (auto &it : _paths) {
    replaceHome(it);

    if (fs::exists((it) / "gcc")) {
      _cCompilers.emplace_back((it) / "gcc");
    }
    if (fs::exists((it) / "gcc.exe")) {
      _cCompilers.emplace_back((it) / "gcc.exe");
    }
    if (fs::exists((it) / "gcc-8")) {
      _cCompilers.emplace_back((it) / "gcc-8");
    }
    if (fs::exists((it) / "gcc-7")) {
      _cCompilers.emplace_back((it) / "gcc-7");
    }
    if (fs::exists((it) / "gcc-6")) {
      _cCompilers.emplace_back((it) / "gcc-6");
    }
    if (fs::exists((it) / "gcc-5")) {
      _cCompilers.emplace_back((it) / "gcc-5");
    }
    if (fs::exists((it) / "gcc-4")) {
      _cCompilers.emplace_back((it) / "gcc-4");
    }
    if (fs::exists((it) / "clang") && !fs::is_directory((it) / "clang")) {
      _cCompilers.emplace_back((it) / "clang");
    }
    if (fs::exists((it) / "clang.exe")) {
      _cCompilers.emplace_back((it) / "clang.exe");
    }
    if (fs::exists((it) / "clang-6.0")) {
      _cCompilers.emplace_back((it) / "clang-6.0");
    }
    if (fs::exists((it) / "clang-5.0")) {
      _cCompilers.emplace_back((it) / "clang-5.0");
    }
    if (fs::exists((it) / "clang-4.0")) {
      _cCompilers.emplace_back((it) / "clang-4.0");
    }
    if (fs::exists((it) / "emcc") &&
        (PLATFORM_NAME == "x64-linux" || PLATFORM_NAME == "x64-osx")) {
      _cCompilers.emplace_back((it) / "emcc");
    }
    if (fs::exists((it) / "emcc.bat") &&
        (PLATFORM_NAME == "x86-windows" || PLATFORM_NAME == "x64-windows")) {
      _cCompilers.emplace_back((it) / "emcc.bat");
    }
    if (fs::exists((it) / "cl.exe")) {
      _cCompilers.emplace_back((it) / "cl.exe");
    }
    if (fs::exists((it) / "g++")) {
      _cppCompilers.emplace_back((it) / "g++");
    }
    if (fs::exists((it) / "g++.exe")) {
      _cppCompilers.emplace_back((it) / "g++.exe");
    }
    if (fs::exists((it) / "/g++-8")) {
      _cppCompilers.emplace_back((it) / "/g++-8");
    }
    if (fs::exists((it) / "g++-7")) {
      _cppCompilers.emplace_back((it) / "g++-7");
    }
    if (fs::exists((it) / "g++-6")) {
      _cppCompilers.emplace_back((it) / "g++-6");
    }
    if (fs::exists((it) / "g++-5")) {
      _cppCompilers.emplace_back((it) / "g++-5");
    }
    if (fs::exists((it) / "clang++")) {
      _cppCompilers.emplace_back((it) / "clang++");
    }
    if (fs::exists((it) / "clang++.exe")) {
      _cppCompilers.emplace_back((it) / "clang++.exe");
    }
    if (fs::exists((it) / "cl.exe")) {
      _cppCompilers.emplace_back((it) / "cl.exe");
    }
    if (fs::exists((it) / "em++") &&
        (PLATFORM_NAME == "x64-linux" || PLATFORM_NAME == "x64-osx")) {
      _cppCompilers.emplace_back((it) / "em++");
    }
    if (fs::exists((it) / "em++.bat") &&
        (PLATFORM_NAME == "x86-windows" || PLATFORM_NAME == "x64-windows")) {
      _cppCompilers.emplace_back((it) / "em++.bat");
    }
    if (fs::exists((it) / "cmake") && !fs::is_directory((it) / "cmake")) {
      _cmakePaths.emplace_back((it) / "cmake");
    }
    if (fs::exists((it) / "cmake.exe")) {
      _cmakePaths.emplace_back((it) / "cmake.exe");
    }
    if (fs::exists((it) / "make")) {
      _makePaths.emplace_back((it) / "make");
    }
    if (fs::exists((it) / "make.exe")) {
      _makePaths.emplace_back((it) / "make.exe");
    }
    if (fs::exists((it) / "nmake.exe")) {
      _makePaths.emplace_back((it) / "nmake.exe");
    }
    if (fs::exists((it) / "mingw32-make.exe")) {
      _makePaths.emplace_back((it) / "mingw32-make.exe");
    }
    if (fs::exists((it) / "ar")) {
      _archiverPaths.emplace_back((it) / "ar");
    }
    if (fs::exists((it) / "ar.exe")) {
      _archiverPaths.emplace_back((it) / "ar.exe");
    }
    if (fs::exists((it) / "lib.exe")) {
      _archiverPaths.emplace_back((it) / "lib.exe");
    }
    if (fs::exists((it) / "emar") &&
        (PLATFORM_NAME == "x64-linux" || PLATFORM_NAME == "x64-osx")) {
      _archiverPaths.emplace_back((it) / "emar");
    }
    if (fs::exists((it) / "emar.bat") &&
        (PLATFORM_NAME == "x86-windows" || PLATFORM_NAME == "x64-windows")) {
      _archiverPaths.emplace_back((it) / "emar.bat");
    }
    if (fs::exists((it) / "vcpkg") && !fs::is_directory((it) / "vcpkg")) {
      _vcpkgPaths.emplace_back((it) / "vcpkg");
    }
    if (fs::exists((it) / "vcpkg.exe")) {
      _vcpkgPaths.emplace_back((it) / "vcpkg.exe");
    }
  }

  sorter(_cCompilers);
  sorter(_cppCompilers);
  sorter(_vcpkgPaths);
  sorter(_cmakePaths);
  sorter(_makePaths);
  sorter(_archiverPaths);
  sorter(_vcpkgPaths);
}

void Init::setup() {
  if (!fs::exists("conf.json")) {
    posixify(_cCompilerPath);
    posixify(_cppCompilerPath);
    posixify(_cmakePath);
    posixify(_makePath);
    posixify(_archiverPath);
    posixify(_vcpkgPath);

    try {
      std::ofstream confOutput("conf.json");
      if (confOutput.is_open()) {
        confOutput << std::setw(4) << "{\n\t\"cCompilerPath\" : \""
                   << _cCompilerPath << "\",\n\t"
                   << R"("cppCompilerPath" : ")" << _cppCompilerPath
                   << "\",\n\t"
                   << R"("vcpkgPath" : ")" << _vcpkgPath << "\",\n\t"
                   << R"("architecture" : ")" << PLATFORM_NAME << "\",\n\t"
                   << R"("cmakePath" : ")" << _cmakePath << "\",\n\t"
                   << R"("makePath" : ")" << _makePath << "\",\n\t"
                   << R"("archiverPath" : ")" << _archiverPath << "\"\n}";
        confOutput.flush();
        confOutput.close();
        std::cout << "conf.json written successfully." << std::endl;
      } else {
        std::cout << "Error opening conf.json." << std::endl;
      }
    } catch (const std::exception &e) {
      std::cerr << e.what() << " " << errno << std::endl;
    }
  }
}

void Init::init(const BinType &binType) {
  std::string binaryType;
  if (binType == BinType::Application) {
    binaryType = "app";
  } else if (binType == BinType::DynamicLibrary) {
    binaryType = "dynamicLibrary";
  } else if (binType == BinType::StaticLibrary) {
    binaryType = "staticLibrary";
  } else {
    binaryType = "app";
  }

  if (!fs::exists("vcbld.json")) {
    try {
      std::ofstream vcbldOutput("vcbld.json");
      if (vcbldOutput.is_open()) {
        vcbldOutput << std::setw(4) << "{\n\t\"projectName\" : "
                    << "\"" << fs::current_path().filename().string()
                    << "\",\n\t"
                    << "\"version\" : "
                    << "\"0.1.0\",\n\t"
                    << "\"language\" : "
                    << "\"c++\",\n\t"
                    << "\"standard\" : "
                    << "\"11\",\n\t"
                    << "\"binaryName\" : "
                    << "\"" << fs::current_path().filename().string()
                    << "\",\n\t"
                    << "\"binaryType\" : "
                    << "\"" << binaryType << "\",\n\t"
                    << "\"sourceDirectory\" : "
                    << "\"src\",\n\t"
                    << "\"outputDirectory\" : "
                    << "\"bin\",\n\t"
                    << "\"includeDirectory\" : "
                    << "\"include\",\n\t"
                    << "\"libDirectory\" : "
                    << "\"lib\",\n\t"
                    << "\"compilerDefines\" : "
                    << "[],\n\t"
                    << "\"compilerFlags\" : "
                    << "[],\n\t"
                    << "\"linkerFlags\" : "
                    << "[]\n"
                    << "}";
        vcbldOutput.flush();
        vcbldOutput.close();
        std::cout << "vcbld.json written successfully." << std::endl;
      } else {
        ConfClass confClass;
        if (!fs::exists(confClass.outputDirectory())) {
          fs::create_directory(confClass.outputDirectory());
          fs::create_directory(confClass.outputDirectory() + "/debug");
          fs::create_directory(confClass.outputDirectory() + "/release");
          std::cout << "output directory created successfully." << std::endl;
        }

        if (!confClass.includeDirectory().empty() &&
            !fs::exists(confClass.includeDirectory())) {
          fs::create_directory(confClass.includeDirectory());
          std::cout << "include directory created successfully." << std::endl;
        }

        if (!confClass.libDirectory().empty() &&
            !fs::exists(confClass.libDirectory())) {
          fs::create_directory(confClass.libDirectory());
          fs::create_directory(confClass.libDirectory() + "/debug");
          fs::create_directory(confClass.libDirectory() + "/release");
          std::cout << "lib directory created successfully." << std::endl;
        }
      }
    } catch (const std::exception &e) {
      std::cerr << e.what() << " " << errno << std::endl;
    }
  }

  if (!fs::exists("package.json")) {
    try {
      std::ofstream pkgsOutput("package.json");
      if (pkgsOutput.is_open()) {
        pkgsOutput << std::setw(4) << "{\n\t\"packages\" : []\n}";
        pkgsOutput.flush();
        pkgsOutput.close();
        std::cout << "package.json written successfully." << std::endl;
      } else {
        std::cout << "package.json exists." << std::endl;
      }
    } catch (const std::exception &e) {
      std::cerr << e.what() << " " << errno << std::endl;
    }
  }
}

void Init::findPathDirs(std::string &PATH, std::vector<fs::path> &dirs) {
  std::string rem = PATH;
  size_t foundSep;
  std::string sep;
  if (rem.find(';') != std::string::npos) {
    sep = ";";
    foundSep = rem.find_first_of(sep);
  } else {
    sep = ":";
    foundSep = rem.find_first_of(sep);
  }
  dirs.emplace_back(rem.substr(0, foundSep));
  rem = rem.substr(foundSep + 1, rem.length());
  if (rem.find_last_of(sep) != std::string::npos) {
    findPathDirs(rem, dirs);
  }
}

void Init::replaceHome(fs::path &path) {
  const char *home = std::getenv("HOME");
  std::string temp = path.string();
  if (home != nullptr) {
    if (temp[0] == '~') {
      temp.replace(0, 1, home);
      path = temp;
    }
  }
}

fs::path Init::findCmake(const fs::path &dir) {
  fs::path temp;
  std::string fileName = "bin";
  const fs::recursive_directory_iterator end;
  try {
    const auto it = std::find_if(fs::recursive_directory_iterator(dir), end,
                                 [&fileName](const fs::directory_entry &e) {
                                   return e.path().filename() == fileName;
                                 });
    if (it == end) {
      return temp;
    } else {
      temp = fs::canonical(it->path());
    }
  } catch (...) {
    // fail quietly
  }
  return temp;
}

void Init::posixify(std::string &path) {
  if (path.find('\\') != std::string::npos) {
    path.replace(path.find('\\'), 1, "/");
    posixify(path);
  }
}

void Init::finder(std::vector<fs::path> &vector, const fs::path &dir) {
  if (fs::is_directory(dir)) {
    std::vector<fs::directory_entry> dirEntry;
    std::copy(fs::directory_iterator(dir), fs::directory_iterator(),
              back_inserter(dirEntry));
    for (auto &it : dirEntry) {
      if (it.path().string().find("MinGW") != std::string::npos ||
          it.path().string().find("cmake") != std::string::npos) {
        vector.emplace_back(it.path() / "bin");
      }
      if (fs::exists((it.path() / "vcpkg").string())) {
        vector.emplace_back(it.path().string());
        if (fs::exists(it.path() / "downloads" / "tools")) {
          vector.emplace_back(findCmake(it.path() / "downloads" / "tools"));
        }
      }
    }
  }
}

void Init::sorter(std::vector<fs::path> &vector) {
  std::sort(vector.begin(), vector.end());
  vector.erase(std::unique(vector.begin(), vector.end()), vector.end());
}

void Init::lister(std::vector<fs::path> &vector) {
  int i = 1;
  for (auto &it : vector) {
    std::cout << i << ") " << it.string() << std::endl;
    i++;
  }
}

std::string Init::chooser(std::vector<fs::path> &vector, const size_t &choice) {
  fs::path temp;
  if (vector.empty()) {
    temp = "";
  } else if (vector.size() == 1) {
    temp = vector[0];
  } else {
    try {
      if (choice <= vector.size() && choice != 0) {
        temp = vector[choice - 1];
      } else {
        temp = vector[0];
      }
    } catch (...) {
      temp = vector[0];
    }
  }
  if (temp != "") {
    try {
      temp = fs::canonical(temp);
    } catch (...) {
      //
    }
  }
  return temp.string();
}

void Init::setCompiler(const int &i) {
  if (!_cCompilers.empty()) {
    _cCompilerPath = chooser(_cCompilers, i);
  }
}
void Init::setCppCompiler(const int &i) {
  if (!_cppCompilers.empty()) {
    _cppCompilerPath = chooser(_cppCompilers, i);
  }
}
void Init::setCmake(const int &i) {
  if (!_cmakePaths.empty()) {
    _cmakePath = chooser(_cmakePaths, i);
  }
}
void Init::setMake(const int &i) {
  if (!_makePaths.empty()) {
    _makePath = chooser(_makePaths, i);
  }
}
void Init::setArchiver(const int &i) {
  if (!_archiverPaths.empty()) {
    _archiverPath = chooser(_archiverPaths, i);
  }
}
void Init::setVcpkg(const int &i) {
  if (!_vcpkgPaths.empty()) {
    _vcpkgPath = chooser(_vcpkgPaths, i);
  }
}

std::vector<fs::path> &Init::cCompilers() { return _cCompilers; }
std::vector<fs::path> &Init::cppCompilers() { return _cppCompilers; }
std::vector<fs::path> &Init::cmakePaths() { return _cmakePaths; }
std::vector<fs::path> &Init::makePaths() { return _makePaths; }
std::vector<fs::path> &Init::archiverPaths() { return _archiverPaths; }
std::vector<fs::path> &Init::vcpkgPaths() { return _vcpkgPaths; }

} // namespace vcbld
