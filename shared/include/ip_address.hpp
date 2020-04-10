#pragma once
#include <string>

#include "caf/all.hpp"

namespace e3::shared {
/// Determines a non localhost IPv4 address of this system.
/// @return A non localhost IPv4 address of this system on success; otherwise an
///         empty optional.
caf::optional<std::string> ip_address();
} // namespace e3::shared
