#ifndef HELPER_H
#define HELPER_H

#include <QString>
#include <functional>
#include <string>

namespace Helper {

QString exec(std::function<void(void)> const &func);

QString execArgs(std::function<void(const std::string &)> const &func,
                 const std::string &arg);

QString execVec(std::function<void(const std::vector<std::string> &)> const &func,
                const std::vector<std::string> &v);
} // namespace Helper

#endif // HELPER_H
