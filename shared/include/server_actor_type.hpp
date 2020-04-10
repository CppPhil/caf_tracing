#pragma once
#include <string>
#include <vector>

#include "caf/all.hpp"

#include "client_actor_type.hpp"

namespace shared {
using server_actor_type = caf::typed_actor
                          < caf::reacts_to<caf::join_atom, std::string,
                                           client_actor_type, std::string>,
      caf::reacts_to<chat_atom, std::string, std::string>,
      caf::reacts_to<caf::leave_atom, std::string, std::string>,
      caf::replies_to<ls_atom, std::string>::with<
        std::pair<std::vector<std::string>, std::string>>;
} // namespace shared
