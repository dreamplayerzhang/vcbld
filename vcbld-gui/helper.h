#ifndef HELPER_H
#define HELPER_H

#include <functional>
#include <QString>
#include <string>

namespace Helper {

QString exec(std::function<void(void)>func);

QString execArgs(std::function<void (const std::string &)>func, const std::string &arg);

QString execVec(std::function<void (const std::vector<std::string> &)>func, const std::vector<std::string> &v);
}

#endif // HELPER_H
