#include "gen.h"

#include <fstream>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <nlohmann/json.hpp>
#include <sstream>
#include <string>
#include <vector>

#include "conf.h"

namespace fs = boost::filesystem;
using json = nlohmann::json;

namespace vcbld::gen
{

void includePathGen(const fs::path &vcbldPath)
{
  json incJson;
  ConfClass confClass(vcbldPath);
  incJson.push_back(confClass.sourceDirectory().relative_path().string());
  incJson.push_back(confClass.vcpkgDirPath() + "/" + "installed" + "/" + "include");
  for (std::vector<std::string>::iterator it = confClass.packageNames.begin();
       it != confClass.packageNames.end(); ++it)
  {
    incJson.push_back(confClass.vcpkgDirPath() + "/" + "packages" + "/" + *it + "/" + confClass.architecture() + "/" + "include");
  }
  std::string temp =
      confClass.sourceDirectory().string();

  if (!fs::exists("includePath.txt"))
  {
    std::ofstream ofs("includePath.txt");

    if (ofs.is_open())
    {
      ofs << std::setw(4) << incJson;
      ofs.flush();
      ofs.close();
    }
    std::cout << "includePath.json file written successfully." << std::endl;
  }
  else
  {
    std::cout << "includePath.json file found." << std::endl;
  }
}

void cmakeGen(const fs::path &vcbldPath)
{
  ConfClass confClass(vcbldPath);

  if (!fs::exists("CMakeLists.txt"))
  {
    std::ofstream ofs("CMakeLists.txt");

    if (ofs.is_open())
    {
      ofs << "cmake_minimum_required(VERSION 3.10.0)\n"
          << "set(CMAKE_CXX_STANDARD " << confClass.standard() << ")\n\n"
          << "project(" << confClass.projectName() << ")\n\n"
          << "include(" << confClass.vcpkgDirPath() << "/scripts/buildsystems/vcpkg.cmake)\n\n"
          << "add_subdirectory(" << confClass.sourceDirectory() << ")\n\n";
      ofs.flush();
      ofs.close();
    }
    std::cout << "CMakeLists.txt file written successfully in parent directory." << std::endl;
  }
  else
  {
    std::cout
        << "A CMakeLists.txt file was found in the parent directory. You can "
           "modify it directly or delete/rename it to regenerate a new file.\n"
        << std::endl;
  }

  if (!fs::exists(confClass.sourceDirectory().string() + "/CMakeLists.txt"))
  {
    std::ofstream ofs(confClass.sourceDirectory().string() + "/CMakeLists.txt");

    if (ofs.is_open())
    {
      if (confClass.language() == "c++")
      {
        ofs << "set(CMAKE_CXX_STANDARD " << confClass.standard() << ")\n"
            << "set(CMAKE_CXX_STANDARD_REQUIRED ON)\n";
      }
      else
      {
        ofs << "set(CMAKE_C_STANDARD " << confClass.standard() << ")\n"
            << "set(CMAKE_C_STANDARD_REQUIRED ON)\n";
      }
      ofs << "set(CMAKE_INCLUDE_CURRENT_DIR ON)"
          << "\n";
      if (confClass.binaryType() == "app")
      {
        ofs << "set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "
               "${PROJECT_BINARY_DIR}/cmake_bin)"
            << "\n";
      }
      else if (confClass.binaryType() == "statLib")
      {
        ofs << "set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "
               "${PROJECT_BINARY_DIR}/cmake_statLib)"
            << "\n";
      }
      else
      {
        ofs << "set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "
               "${PROJECT_BINARY_DIR}/cmake_dyLib)"
            << "\n";
      }
      ofs << "set(CMAKE_EXPORT_COMPILE_COMMANDS ON)"
          << "\n"
          << "set(CMAKE_CXX_FLAGS "
          << "\"" << confClass.compilerFlags() << "\")\n"
          << "add_definitions(" << confClass.compilerDefines() << ")\n\n"
          << "#Uncomment the following lines if you're using Qt QObject macro, Qt qrc or Qt widgets.\n"
          << "#set(CMAKE_AUTOMOC ON)\n#set(CMAKE_AUTOUIC "
             "ON)\n#set(CMAKE_AUTORCC ON)\n\n"
          << "if(${CMAKE_SOURCE_DIR} STREQUAL "
             "${CMAKE_BINARY_DIR})\n\tmessage(FATAL_ERROR \"Prevented in-tree "
             "built. Please create a build directory outside of the source "
             "code and call cmake from there. Thank you.\")\nendif()\n\n"
          << "set(SOURCEFILES " << confClass.sourceFilesSinPath()
          << ")\n\n"
          << "set(DBG_LIB_PATH ${_VCPKG_ROOT_DIR}/installed/${VCPKG_TARGET_TRIPLET}/debug/lib)\n"
          << "set(RLS_LIB_PATH ${_VCPKG_ROOT_DIR}/installed/${VCPKG_TARGET_TRIPLET}/lib)\n\n";

      for (std::vector<std::string>::iterator it = confClass.libs.begin(); it != confClass.libs.end(); ++it)
      {
        std::string libName = *it;
        std::string module = libName;
        module[0] = toupper(module[0]);
        ofs << "find_library(" << libName << "_DBG NAMES " << libName << " HINTS "
            << "${DBG_LIB_PATH})\n"
            << "find_library(" << libName << "_RLS NAMES " << libName << " HINTS "
            << "${RLS_LIB_PATH})\n"
            << "set(dbgLIBS ${dbgLIBS} ${" << libName << "_DBG})\n"
            << "set(rlsLIBS ${rlsLIBS} ${" << libName << "_RLS})\n\n";
      }

      if (confClass.binaryType() == "app")
      {
        ofs << "add_executable(${PROJECT_NAME} ${SOURCEFILES})\n\n";
      }
      else if (confClass.binaryType() == "statLib")
      {
        ofs << "add_library(${PROJECT_NAME} STATIC ${SOURCEFILES})\n\n";
      }
      else
      {
        ofs << "add_library(${PROJECT_NAME} SHARED ${SOURCEFILES})\n\n";
      }

      ofs << "target_include_directories(${PROJECT_NAME} PUBLIC ${_VCPKG_ROOT_DIR}/installed/${VCPKG_TARGET_TRIPLET}"
          << "/include)\n"
          << "target_link_libraries(${PROJECT_NAME} debug ${dbgLIBS})\n"
          << "target_link_libraries(${PROJECT_NAME} optimized ${rlsLIBS})\n";
      ofs.flush();
      ofs.close();
    }
    std::cout << "CMakeLists.text file written successfully in source directory." << std::endl;
  }
  else
  {
    std::cout
        << "A CMakeLists.txt file was found in the source directory. You can "
           "modify it directly or delete/rename it to regenerate a new file.\n"
        << std::endl;
  }
}

} // namespace vcbld::gen
