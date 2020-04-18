#pragma once
#include <caf/all.hpp>

#include "span_context.hpp"

namespace shared::traced {
template <class... Ts>
using reacts_to = caf::reacts_to<Ts..., span_context>;
} // namespace shared::traced
