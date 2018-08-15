#ifndef PKG_H
#define PKG_H

#include <string>
#include <vector>

namespace vcbld
{
class PkgClass
{
  public:
    PkgClass(const std::string &pkg);
    std::string packageName() const;
    std::string version() const;

    std::vector<std::string> libNames;

  private:
    std::string _packageName;
    std::string _version;
};
} // namespace vcbld
#endif // !PKG_H
