#pragma once

#if defined(_WIN32) || defined(_WIN64)
#include <filesystem>
#else
#include <experimental/filesystem>
#endif
#include "args.h"
#include "builder.h"
#include "conf.h"
#include "gen.h"
#include "help.h"
#include "init.h"
#include "pkg.h"
#include "prep.h"

#include "catch.hpp"