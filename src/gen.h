#ifndef GEN_H
#define GEN_H

#include <boost/filesystem.hpp>

namespace fs = boost::filesystem;

namespace vcbld::gen {
void includePathGen(const fs::path &vcbldPath);
void cmakeGen(const fs::path &vcbldPath);
} // namespace vcbld::gen
#endif // !GEN_H