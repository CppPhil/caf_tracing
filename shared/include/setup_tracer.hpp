#pragma once
#include <string>

namespace shared {
void setup_tracer(const std::string& config_filepath,
                  const std::string& service);
} // namespace shared
