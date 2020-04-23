#pragma once
#include <string>
#include <vector>

#include <caf/all.hpp>

#include "traced/reacts_to.hpp"
#include "traced/replies_to.hpp"
#include "types.hpp"

namespace shared {
using server_actor_type = caf::typed_actor<
  traced::reacts_to<caf::join_atom, std::string, client_actor_type>,
  traced::reacts_to<chat_atom, std::string>,
  traced::reacts_to<caf::leave_atom, std::string>,
  traced::replies_to<ls_atom>::with<std::vector<std::string>>>;
} // namespace shared
