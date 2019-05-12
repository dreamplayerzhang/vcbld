#include "gen.h"
#include "pch.h"
#include "pkg.h"
#include "prep.h"

namespace vcbld {
namespace gen {
void includePathGen() {
  json incJson;
  PkgClass pkgClass;
  incJson.push_back(pkgClass.sourceDirectory());
  if (!pkgClass.includeDirectory().empty()) {
    incJson.push_back(pkgClass.includeDirectory());
  }
  incJson.push_back(pkgClass.outputDirectory());
  incJson.push_back(pkgClass.vcpkgDirPath() + "/" + "installed" + "/" +
                    "include");
  std::string temp = pkgClass.sourceDirectory();

  if (!fs::exists("includePath.json")) {
    try {
      std::ofstream ofs("includePath.json");
      if (ofs.is_open()) {
        ofs << std::setw(4) << incJson;
        ofs.flush();
        ofs.close();
      }
      std::cout << "includePath.json file written successfully." << std::endl;
    } catch (const std::exception &e) {
      std::cerr << e.what() << " " << errno << std::endl;
    }
  } else {
    std::cout << "includePath.json file found." << std::endl;
  }
}

void cmakeGen() {
  PrepClass prepClass;

  if (!fs::exists("CMakeLists.txt")) {
    try {
      std::ofstream ofs("CMakeLists.txt");
      if (ofs.is_open()) {
        ofs << "cmake_minimum_required(VERSION 3.10.0)\n"
            << "set(CMAKE_CXX_STANDARD " << prepClass.standard() << ")\n\n"
            << "project(" << prepClass.projectName() << " VERSION "
            << prepClass.version() << ")\n\n"
            << "add_subdirectory(" << prepClass.sourceDirectory() << ")\n\n";
        ofs.flush();
        ofs.close();
      }
      std::cout
          << "CMakeLists.txt file written successfully in parent directory."
          << std::endl;
    } catch (const std::exception &e) {
      std::cerr << e.what() << " " << errno << std::endl;
    }
  } else {
    std::cout
        << "\nA CMakeLists.txt file was found in the parent directory. You can "
           "modify it directly or delete/rename it to regenerate a new file.\n"
        << std::endl;
  }

  if (!fs::exists(prepClass.sourceDirectory() + "/CMakeLists.txt")) {
    try {
      std::ofstream ofs(prepClass.sourceDirectory() + "/CMakeLists.txt");
      if (ofs.is_open()) {
        if (prepClass.language() == "c++") {
          ofs << "set(CMAKE_CXX_STANDARD " << prepClass.standard() << ")\n"
              << "set(CMAKE_CXX_STANDARD_REQUIRED ON)\n";
        } else {
          ofs << "set(CMAKE_C_STANDARD " << prepClass.standard() << ")\n"
              << "set(CMAKE_C_STANDARD_REQUIRED ON)\n";
        }
        ofs << "set(CMAKE_INCLUDE_CURRENT_DIR ON)"
            << "\n";
        if (prepClass.binaryType() == "app") {
          ofs << "set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG "
                 "${PROJECT_BINARY_DIR}/debug)\n"
              << "set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE "
                 "${PROJECT_BINARY_DIR}/release)\n";
        } else if (prepClass.binaryType() == "statLib") {
          ofs << "set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG "
                 "${PROJECT_BINARY_DIR}/debug)\n"
              << "set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE "
                 "${PROJECT_BINARY_DIR}/release)\n";
        } else {
          ofs << "set(CMAKE_LIBRARY_OUTPUT_DIRECTORY _DEBUG "
                 "${PROJECT_BINARY_DIR}/debug)\n"
              << "set(CMAKE_LIBRARY_OUTPUT_DIRECTORY _RELEASE "
                 "${PROJECT_BINARY_DIR}/release)\n";
        }
        ofs << "set(CMAKE_EXPORT_COMPILE_COMMANDS ON)"
            << "\n"
            << "set(CMAKE_CXX_FLAGS " << prepClass.compilerFlags() << ")\n"
            << "add_definitions(" << prepClass.compilerDefines() << ")\n"
            << "set(CMAKE_EXE_LINKER_FLAGS " << prepClass.linkerFlags()
            << ")\n\n"
            << "if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)\n\t"
            << "set(CMAKE_BUILD_TYPE Debug)\n"
            << "endif()\n\n"
            << "#Uncomment the following lines if you're using Qt QObject "
               "macro, "
               "Qt qrc or Qt widgets.\n"
            << "#set(CMAKE_AUTOMOC ON)\n#set(CMAKE_AUTOUIC "
               "ON)\n#set(CMAKE_AUTORCC ON)\n\n"
            << "if(${CMAKE_SOURCE_DIR} STREQUAL "
               "${CMAKE_BINARY_DIR})\n\tmessage(FATAL_ERROR \"Prevented "
               "in-tree "
               "build. Please create a build directory outside of the source "
               "code and call cmake from there. Thank you.\")\nendif()\n\n"
            << "set(SOURCE_FILES " << prepClass.sourceFilesSinPath() << ")\n\n";

        if (!prepClass.libDirectory().empty() &&
            fs::exists(prepClass.libDirectory())) {
          ofs << "set(LOCAL_DBG_LIB_PATH "
                 "${CMAKE_CURRENT_SOURCE_DIR}/../"
              << fs::path(prepClass.libDirectory()).filename().string()
              << "/debug)\n"
              << "set(LOCAL_RLS_LIB_PATH "
                 "${CMAKE_CURRENT_SOURCE_DIR}/../"
              << fs::path(prepClass.libDirectory()).filename().string()
              << "/release)\n";
        }
        ofs << "set(VCPKG_DBG_LIB_PATH "
               "${CMAKE_PREFIX_PATH}/debug/"
               "lib NO_DEFAULT_PATH)\n"
            << "set(VCPKG_RLS_LIB_PATH "
               "${CMAKE_PREFIX_PATH}/lib NO_DEFAULT_PATH)\n\n"
            << prepClass.cmakeOutput() << "\n";

        if (prepClass.binaryType() == "app") {
          ofs << "add_executable(" << prepClass.binaryName()
              << " ${SOURCE_FILES})\n\n";
        } else if (prepClass.binaryType() == "statLib") {
          ofs << "add_library(" << prepClass.binaryName()
              << " STATIC ${SOURCE_FILES})\n\n";
        } else {
          ofs << "add_library(" << prepClass.binaryName()
              << " SHARED ${SOURCE_FILES})\n\n";
        }

        ofs << "target_include_directories(" << prepClass.binaryName()
            << " PUBLIC "
               "${CMAKE_PREFIX_PATH}/include)\n";
        if (!prepClass.includeDirectory().empty() &&
            fs::exists(prepClass.includeDirectory())) {
          ofs << "target_include_directories(" << prepClass.binaryName()
              << " PUBLIC "
                 "${CMAKE_CURRENT_SOURCE_DIR}/../"
              << fs::path(prepClass.includeDirectory()).filename().string()
              << ")\n";
        }
        if (!prepClass.fullLibNames().empty() &&
            !prepClass.fullDbgLibNames().empty()) {
          ofs << "target_link_libraries(" << prepClass.binaryName()
              << " debug ${dbgLIBS})\n"
              << "target_link_libraries(" << prepClass.binaryName()
              << " optimized "
                 "${rlsLIBS})\n";
        }
        ofs.flush();
        ofs.close();
      }
      std::cout
          << "CMakeLists.text file written successfully in source directory."
          << std::endl;
    } catch (const std::exception &e) {
      std::cerr << e.what() << " " << errno << std::endl;
    }
  } else {
    std::cout
        << "\nA CMakeLists.txt file was found in the source directory. You can "
           "modify it directly or delete/rename it to regenerate a new file.\n"
        << std::endl;
  }
}
} // namespace gen
} // namespace vcbld
