#ifndef INIT_H
#define INIT_H

#include <string>
#include <boost/filesystem.hpp>

namespace fs = boost::filesystem;

namespace vcbld::init
{
void init(const std::string &binType);
void setup(const fs::path &vcbldPath);
} // namespace vcbld::init
#endif // !INIT_H
