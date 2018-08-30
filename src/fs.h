#ifndef FS_H
#define FS_H 

#if defined(_WIN32) || defined(_WIN64)
#include <filesystem>
#else
#include <experimental/filesystem>
#endif

namespace fs = std::experimental::filesystem;

#endif // !FS_H
