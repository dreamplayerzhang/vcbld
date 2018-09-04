#include "qpch.h"

namespace Helper {
QString exec(std::function<void(void)> func) {
  std::stringstream ss;
  auto oldBuf = std::cout.rdbuf(ss.rdbuf());
  func();
  std::cout.rdbuf(oldBuf);
  return QString::fromStdString(ss.str());
}

QString execArgs(std::function<void(const std::string &)> func,
                 const std::string &arg) {
  std::stringstream ss;
  auto oldBuf = std::cout.rdbuf(ss.rdbuf());
  func(arg);
  std::cout.rdbuf(oldBuf);
  return QString::fromStdString(ss.str());
}

QString execVec(std::function<void(const std::vector<std::string> &)> func,
                const std::vector<std::string> &v) {
  std::stringstream ss;
  auto oldBuf = std::cout.rdbuf(ss.rdbuf());
  func(v);
  std::cout.rdbuf(oldBuf);
  return QString::fromStdString(ss.str());
}

} // namespace Helper
