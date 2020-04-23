#pragma once
#include <string>

#include <caf/logger.hpp>

#include <pl/stringify.hpp>

#define SHARED_SOURCE_LINE PL_STRINGIFY(__LINE__)

#define SHARED_SOURCE_LOCATION                                                 \
  "file: " + std::string(__FILE__)                                             \
    + "\nline: " SHARED_SOURCE_LINE "\nfunction: "                             \
    + std::string(CAF_PRETTY_FUN)
