#pragma once
#include <string>

#include <caf/logger.hpp>
#include <caf/test/unit_test.hpp>

#define SHARED_SOURCE_LINE CAF_XSTR(__LINE__)

#define SHARED_SOURCE_LOCATION                                                 \
  "file: " + std::string(__FILE__)                                             \
    + "\nline: " SHARED_SOURCE_LINE "\nfunction: "                             \
    + std::string(CAF_PRETTY_FUN)
