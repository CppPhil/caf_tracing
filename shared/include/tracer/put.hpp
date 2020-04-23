#pragma once
#include <string_view>

#include <caf/all.hpp>

namespace shared::tracer {
void put(caf::actor_id aid, std::string_view app);
} // namespace shared::tracer