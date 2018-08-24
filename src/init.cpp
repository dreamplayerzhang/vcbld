#include "init.h"

#include <algorithm>
#include <errno.h>
#if defined(_WIN32) || defined(_WIN64)
#include <filesystem>
#else
#include <experimental/filesystem>
#endif
#include <fstream>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <sstream>
#include <typeinfo>
#include <vector>

#include "conf.h"

#if defined(_WIN32)
#define PLATFORM_NAME "x86-windows"
#define PATHSEP "\\"
#elif defined(_WIN64)
#define PLATFORM_NAME "x64-windows"
#define PATHSEP "\\"
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

namespace fs = std::experimental::filesystem;

namespace vcbld {
namespace init {
void setup(const fs::path &vcbldPath) {
  std::string PATH = std::getenv("PATH");
  std::string brewLLVM = "/usr/local/opt/llvm/bin";
  std::vector<fs::path> paths, cCompilers, cppCompilers, cmakePaths,
      makePaths, archiverPaths, vcpkgPaths;
  std::string cCompilerPath, cppCompilerPath, cmakePath, makePath, archiverPath,
      vcpkgPath;

  findPathDirs(PATH, paths);

  if (fs::exists(brewLLVM)) {
    paths.emplace_back(brewLLVM);
  }
  if (fs::exists(vcbldPath)) {
    paths.emplace_back(fs::canonical(vcbldPath));
    paths.emplace_back(
        findCmake(fs::canonical(vcbldPath) / "downloads" / "tools"));
  }

  try {
    finder(paths, "C:/");
    if (std::getenv("HOME") != nullptr)
      finder(paths, std::getenv("HOME"));
    if (std::getenv("PROGRAMFILES") != nullptr)
      finder(paths, std::getenv("PROGRAMFILES"));
    if (std::getenv("MINGW_HOME") != nullptr)
      finder(paths, std::getenv("MINGW_HOME"));
  } catch (...) {
    // fail quietly
  }

  std::sort(paths.begin(), paths.end());
  paths.erase(std::unique(paths.begin(), paths.end()), paths.end());

  for (std::vector<fs::path>::iterator it = paths.begin(); it != paths.end();
       ++it) {
    replaceHome(*it);

    if (fs::exists((*it) / "gcc")) {
      cCompilers.emplace_back((*it) / "gcc");
    }
    if (fs::exists((*it) / "gcc.exe")) {
      cCompilers.emplace_back((*it) / "gcc.exe");
    }
    if (fs::exists((*it) / "gcc-8")) {
      cCompilers.emplace_back((*it) / "gcc-8");
    }
    if (fs::exists((*it) / "gcc-7")) {
      cCompilers.emplace_back((*it) / "gcc-7");
    }
    if (fs::exists((*it) / "gcc-6")) {
      cCompilers.emplace_back((*it) / "gcc-6");
    }
    if (fs::exists((*it) / "gcc-5")) {
      cCompilers.emplace_back((*it) / "gcc-6");
    }
    if (fs::exists((*it) / "clang") && !fs::is_directory((*it) / "clang")) {
      cCompilers.emplace_back((*it) / "clang");
    }
    if (fs::exists((*it) / "clang.exe")) {
      cCompilers.emplace_back((*it) / "clang.exe");
    }
    if (fs::exists((*it) / "clang-6.0")) {
      cCompilers.emplace_back((*it) / "clang-6.0");
    }
    if (fs::exists((*it) / "clang-5.0")) {
      cCompilers.emplace_back((*it) / "clang-5.0");
    }
    if (fs::exists((*it) / "clang-4.0")) {
      cCompilers.emplace_back((*it) / "clang-4.0");
    }
    if (fs::exists((*it) / "g++")) {
      cppCompilers.emplace_back((*it) / "g++");
    }
    if (fs::exists((*it) / "g++.exe")) {
      cppCompilers.emplace_back((*it) / "g++.exe");
    }
    if (fs::exists((*it) / "/g++-8")) {
      cppCompilers.emplace_back((*it) / "/g++-8");
    }
    if (fs::exists((*it) / "g++-7")) {
      cppCompilers.emplace_back((*it) / "g++-7");
    }
    if (fs::exists((*it) / "g++-6")) {
      cppCompilers.emplace_back((*it) / "g++-6");
    }
    if (fs::exists((*it) / "clang++")) {
      cppCompilers.emplace_back((*it) / "clang++");
    }
    if (fs::exists((*it) / "clang++.exe")) {
      cppCompilers.emplace_back((*it) / "clang++.exe");
    }
    if (fs::exists((*it) / "cmake") && !fs::is_directory((*it) / "cmake")) {
      cmakePaths.emplace_back((*it) / "cmake");
    }
    if (fs::exists((*it) / "cmake.exe")) {
      cmakePaths.emplace_back((*it) / "cmake.exe");
    }
    if (fs::exists((*it) / "make")) {
      makePaths.emplace_back((*it) / "make");
    }
    if (fs::exists((*it) / "make.exe")) {
      makePaths.emplace_back((*it) / "make.exe");
    }
    if (fs::exists((*it) / "mingw32-make.exe")) {
      makePaths.emplace_back((*it) / "mingw32-make.exe");
    }
    if (fs::exists((*it) / "ar")) {
      archiverPaths.emplace_back((*it) / "ar");
    }
    if (fs::exists((*it) / "ar.exe")) {
      archiverPaths.emplace_back((*it) / "ar.exe");
    }
    if (fs::exists((*it) / "vcpkg") && !fs::is_directory((*it) / "vcpkg")) {
      vcpkgPaths.emplace_back((*it) / "vcpkg");
    }
    if (fs::exists((*it) / "vcpkg.exe")) {
      vcpkgPaths.emplace_back((*it) / "vcpkg.exe");
    }
  }

  if (fs::exists("conf.json")) {
    std::cout << "conf.json was found in the directory, would you like to "
                 "reconfigure it?\ntype yes to reconfigure..."
              << std::endl;
    std::string entry;
    std::cin >> entry;
    if (entry == "yes") {
      fs::remove("conf.json");
    }
  }

  if (!fs::exists("conf.json")) {
    cCompilerPath = chooser(cCompilers, " C compiler.");
    cppCompilerPath = chooser(cppCompilers, " C++ compiler.");
    cmakePath = chooser(cmakePaths, " cmake executable.");
    makePath = chooser(makePaths, " make executable.");
    archiverPath = chooser(archiverPaths, "n archiver.");
    vcpkgPath = chooser(vcpkgPaths, " vcpkg executable.");
    posixify(cCompilerPath);
    posixify(cppCompilerPath);
    posixify(cmakePath);
    posixify(makePath);
    posixify(archiverPath);
    posixify(vcpkgPath);

    try {
      std::ofstream confOutput("conf.json");
      if (confOutput.is_open()) {
        confOutput << std::setw(4) << "{\n\t\"cCompilerPath\" : \""
                   << cCompilerPath << "\",\n\t"
                   << "\"cppCompilerPath\" : \"" << cppCompilerPath << "\",\n\t"
                   << "\"vcpkgPath\" : \"" << vcpkgPath << "\",\n\t"
                   << "\"architecture\" : \"" << PLATFORM_NAME << "\",\n\t"
                   << "\"cmakePath\" : \"" << cmakePath << "\",\n\t"
                   << "\"makePath\" : \"" << makePath << "\",\n\t"
                   << "\"archiverPath\" : \"" << archiverPath << "\"\n}";
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

  if (!fs::exists("package.json")) {
    try {
      std::ofstream pkgsOutput("package.json");
      if (pkgsOutput.is_open()) {
        pkgsOutput << std::setw(4) << "{\n\t\"packages\" : []\n}";
        pkgsOutput.flush();
        pkgsOutput.close();
        std::cout << "package.json written successfully." << std::endl;
      }
    } catch (const std::exception &e) {
      std::cerr << e.what() << " " << errno << std::endl;
    }
  }

  if (fs::exists("vcbld.json")) {
    ConfClass confClass;
    if (!fs::exists(confClass.outputDirectory())) {
      fs::create_directory(confClass.outputDirectory());
      fs::create_directory(confClass.outputDirectory() + "/debug");
      fs::create_directory(confClass.outputDirectory() + "/release");
      std::cout << "output directory created successfully." << std::endl;
    }

    if (confClass.includeDirectory() != "") {
      if (!fs::exists(confClass.includeDirectory())) {
        fs::create_directory(confClass.includeDirectory());
        std::cout << "include directory created successfully." << std::endl;
      }
    }

    if (confClass.includeDirectory() != "") {
      if (!fs::exists(confClass.libDirectory()) &&
          confClass.libDirectory() != "") {
        fs::create_directory(confClass.libDirectory());
        fs::create_directory(confClass.libDirectory() + "/debug");
        fs::create_directory(confClass.libDirectory() + "/release");
        std::cout << "lib directory created successfully." << std::endl;
      }
    }
  }
}

void init(const std::string &binType) {
  std::string binaryType;
  if (binType == "app") {
    binaryType = "app";
  } else if (binType == "dylib") {
    binaryType = "dynamicLibrary";
  } else if (binType == "statlib") {
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
        std::cout << "vcbld.json exists." << std::endl;
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

void findPathDirs(std::string &PATH, std::vector<fs::path> &dirs) {
  std::string rem = PATH;
  size_t foundSep;
  std::string sep;
  if (rem.find(";") != std::string::npos) {
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

void replaceHome(fs::path &path) {
  const char *home = std::getenv("HOME");
  std::string temp = path.string();
  if (home != nullptr) {
    if (temp[0] == '~') {
      temp.replace(0, 1, home);
      path = temp;
    }
  }
}

fs::path findCmake(const fs::path &dir) {
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

void posixify(std::string &path) {
  if (path.find("\\") != std::string::npos) {
    path.replace(path.find("\\"), 1, "/");
    posixify(path);
  }
}

void finder(std::vector<fs::path> &vector, const fs::path &dir) {
  if (fs::is_directory(dir)) {
    std::vector<fs::directory_entry> dirEntry;
    std::copy(fs::directory_iterator(dir), fs::directory_iterator(),
              back_inserter(dirEntry));
    for (std::vector<fs::directory_entry>::iterator it = dirEntry.begin();
         it != dirEntry.end(); ++it) {
      if ((*it).path().string().find("MinGW") != std::string::npos ||
          (*it).path().string().find("cmake") != std::string::npos) {
        vector.emplace_back((*it).path() / "bin");
      }
      if (fs::exists(((*it).path() / "vcpkg").string())) {
        vector.emplace_back((*it).path().string());
        if (fs::exists((*it).path() / "downloads" / "tools")) {
          vector.emplace_back(
              findCmake((*it).path() / "downloads" / "tools"));
        }
      }
    }
  }
}

std::string chooser(std::vector<fs::path> &vector, const std::string &cli) {
  std::sort(vector.begin(), vector.end());
  vector.erase(std::unique(vector.begin(), vector.end()), vector.end());
  fs::path temp;
  if (vector.size() == 0) {
    temp = "";
  } else if (vector.size() == 1) {
    temp = vector[0];
  } else {
    int i = 1;
    for (std::vector<fs::path>::const_iterator it = vector.begin();
         it != vector.end(); ++it) {
      std::cout << i << ") " << (*it).string() << std::endl;
      i++;
    }
    std::cout << "Please choose a" << cli << std::endl;
    try {
      int entry;
      std::cin >> entry;
      if (entry <= vector.size() && entry != 0 &&
          typeid(entry) == typeid(int)) {
        temp = vector[entry - 1];
      } else {
        temp = vector[0];
      }
    } catch (...) {
      temp = vector[0];
    }
  }
  if (temp != "") {
    try {
      temp = fs::canonical(temp).string();
    } catch (...) {
		//
    }
  }
  return temp.string();
}

} // namespace init
} // namespace vcbld
