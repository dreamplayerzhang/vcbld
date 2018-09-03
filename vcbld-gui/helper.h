#ifndef HELPER_H
#define HELPER_H

#include <QString>
#include <functional>
#include <string>

namespace Helper {

QString exec(std::function<void(void)> func);

QString execArgs(std::function<void(const std::string &)> func,
                 const std::string &arg);

QString execVec(std::function<void(const std::vector<std::string> &)> func,
                const std::vector<std::string> &v);
} // namespace Helper

#endif // HELPER_H
