#pragma once
#include <caf/all.hpp>

#include "message.hpp"

namespace shared::traced {
template <class... Ts>
using reacts_to = caf::reacts_to<message<Ts...>>;
} // namespace shared::traced
