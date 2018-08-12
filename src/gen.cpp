#include "gen.h"

#include <fstream>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <nlohmann/json.hpp>
#include <sstream>
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
  std::ostringstream includePath;
  includePath << confClass.vcpkgDirPath() << "/"
              << "installed"
              << "/" << confClass.architecture() << "/"
              << "include\n";
  incJson.push_back(confClass.vcpkgDirPath() + "/" + "installed" + "/" + "include");
  for (std::vector<std::string>::iterator it = confClass.packageName.begin();
       it != confClass.packageName.end(); ++it)
  {
    includePath << confClass.vcpkgDirPath() << "/"
                << "packages"
                << "/" << *it << confClass.architecture() << "/"
                << "include\n";
    incJson.push_back(confClass.vcpkgDirPath() + "/" + "packages" + "/" + *it + "/" + confClass.architecture() + "/" + "include");
  }

  std::string temp =
      confClass.sourceDirectory().string() + "/" + "includePath.txt";

  if (!fs::exists(temp))
  {
    std::ofstream ofs(temp);

    if (ofs.is_open())
    {
      ofs << includePath.str();
      ofs.flush();
      ofs.close();
    }
    std::cout << "includePath.txt file written successfully." << std::endl;
  }
  else
  {
    std::cout << "includePath.txt file found." << std::endl;
  }
  std::string temp2 =
      confClass.sourceDirectory().string() + "/" + "includePath.json";
  if (!fs::exists(temp2))
  {
    std::ofstream ofs2(temp2);

    if (ofs2.is_open())
    {
      ofs2 << std::setw(4) << incJson;
      ofs2.flush();
      ofs2.close();
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
          << "\"" << confClass.compilerFlags() << ")\n"
          << "add_definitions(" << confClass.compilerDefines() << ")\n\n"
          << "#set(CMAKE_AUTOMOC ON)\n#set(CMAKE_AUTOUIC "
             "ON)\n#set(CMAKE_AUTORCC ON)\n\n"
          << "if(${CMAKE_SOURCE_DIR} STREQUAL "
             "${CMAKE_BINARY_DIR})\nmessage(FATAL_ERROR \"Prevented in-tree "
             "built. Please create a build directory outside of the source "
             "code and call cmake from there. Thank you.\")\nendif()\n\n"
          << "include_directories PUBLIC ${VCPKG_ROOT}/installed/${VCPKG_TARGET_TRIPLET}"
          << "/include)\n\n";

      if (confClass.binaryType() == "app")
      {
        ofs << "add_executable(${PROJECT_NAME} " << confClass.sourceFilesSinPath()
            << ")\n\n";
      }
      else if (confClass.binaryType() == "statLib")
      {
        ofs << "add_library(${PROJECT_NAME} STATIC " << confClass.sourceFilesSinPath()
            << ")\n\n";
      }
      else
      {
        ofs << "add_library(${PROJECT_NAME} SHARED " << confClass.sourceFilesSinPath()
            << ")\n\n";
      }

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
