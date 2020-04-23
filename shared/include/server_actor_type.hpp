#pragma once
#include <string>
#include <vector>

#include <caf/all.hpp>

#include "types.hpp"

namespace shared {
using server_actor_type = caf::typed_actor<
  caf::reacts_to<caf::join_atom, std::string, client_actor_type>,
  caf::reacts_to<chat_atom, std::string>,
  caf::reacts_to<caf::leave_atom, std::string>,
  caf::replies_to<ls_atom>::with<std::vector<std::string>>>;
} // namespace shared
