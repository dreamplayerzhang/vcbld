#ifndef PREDEFS_H
#define PREDEFS_H


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



enum class Configuration {
	Debug,
	Release
};

enum class BinType {
	Application,
	StaticLibrary,
	DynamicLibrary
};

#endif
