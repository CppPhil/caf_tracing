#pragma once
#include <string>
#include <vector>

#include <caf/all.hpp>

#include "client_actor_type.hpp"
#include "span_context.hpp"

namespace shared {
using server_actor_type = caf::typed_actor<
  caf::reacts_to<caf::join_atom, std::string, client_actor_type, span_context>,
  caf::reacts_to<chat_atom, std::string, span_context>,
  caf::reacts_to<caf::leave_atom, std::string, span_context>,
  caf::replies_to<ls_atom, span_context>::with<std::vector<std::string>,
                                               span_context>>;
} // namespace shared
