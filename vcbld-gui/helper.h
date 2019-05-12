#ifndef HELPER_H
#define HELPER_H

#include <QString>
#include <functional>
#include <string>

#include "predefs.h"

namespace Helper {

QString exec(std::function<void(void)> const &func);

} // namespace Helper

#endif // HELPER_H
