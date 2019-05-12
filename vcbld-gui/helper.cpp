#include "helper.h"
#include "qpch.h"

namespace Helper {
QString exec(const std::function<void(void)> &func) {
  std::stringstream ss;
  auto oldBuf = std::cout.rdbuf(ss.rdbuf());
  func();
  std::cout.rdbuf(oldBuf);
  return QString::fromStdString(ss.str());
}

} // namespace Helper
