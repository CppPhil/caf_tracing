#pragma once
#include <caf/all.hpp>

#include "message.hpp"

namespace shared::traced {
template <class Atom, class... Ts>
using reacts_to = caf::reacts_to<Atom, message<Ts...>>;
} // namespace shared::traced
