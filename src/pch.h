#ifndef PCH_H
#define PCH_H

#if defined(_WIN32) || defined(_WIN64)
#include <filesystem>
#else
#include <experimental/filesystem>
#endif

#include <algorithm>
#include <cstdlib>
#include <cstring>
#include <errno.h>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <nlohmann/json.hpp>
#include <sstream>
#include <string>
#include <typeinfo>
#include <vector>

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

#endif // !PCH_H