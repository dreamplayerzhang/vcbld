#include "gen.h"

#include <fstream>
#include <iostream>
#include <sstream>

#include "conf.h"
#include "pkg.h"

namespace fs = boost::filesystem;

namespace vcbld::gen {

void includePathGen(const fs::path &vcbldPath) {
  ConfClass confClass(vcbldPath);
  PkgClass pkgClass(vcbldPath);
  std::ostringstream includePath;
  includePath << confClass.vcpkgDirPath() << "/"
              << "installed"
              << "/" << confClass.architecture() << "/"
              << "include\n";
  for (std::vector<std::string>::iterator it = pkgClass.packageName.begin();
       it != pkgClass.packageName.end(); ++it) {
    includePath << confClass.vcpkgDirPath() << "/"
                << "packages"
                << "/" << *it << confClass.architecture() << "/"
                << "include\n";
  }

  std::string temp =
      confClass.sourceDirectory().string() + "/" + "includePath.txt";
  std::ofstream ofs(temp);

  if (ofs.is_open()) {
    ofs << includePath.str();
    ofs.flush();
    ofs.close();
  }
}

void cmakeGen(const fs::path &vcbldPath) {
  ConfClass confClass(vcbldPath);
  PkgClass pkgClass(vcbldPath);

  if (!fs::exists("CMakeLists.txt")) {
    std::ofstream ofs("CMakeLists.txt");

    if (ofs.is_open()) {
      ofs << "cmake_minimum_required(VERSION 3.10.0)\n"
          << "set(CMAKE_CXX_STANDARD " << confClass.standard() << ")\n\n"
          << "project(" << confClass.projectName() << ")\n\n"
          << "add_subdirectory(" << confClass.sourceDirectory() << ")\n\n";
      ofs.flush();
      ofs.close();
    }
  } else {
    std::ofstream ofs("tempCMakeLists.txt");

    if (ofs.is_open()) {
      ofs << "cmake_minimum_required(VERSION 3.10.0)\n"
          << "set(CMAKE_CXX_STANDARD " << confClass.standard() << ")\n\n"
          << "project(" << confClass.projectName() << ")\n\n"
          << "add_subdirectory(" << confClass.sourceDirectory() << ")\n\n";
      ofs.flush();
      ofs.close();
    }
  }
}
}