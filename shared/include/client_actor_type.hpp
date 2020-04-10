#pragma once
#include <string>

#include "caf/all.hpp"

#include "atoms.hpp"

namespace shared {
using client_actor_type
  = caf::typed_actor<caf::reacts_to<caf::join_atom, std::string>,
                     caf::reacts_to<ls_atom>,
                     caf::reacts_to<caf::leave_atom, std::string>,
                     caf::reacts_to<local_chat_atom, std::string>,
                     caf::reacts_to<chat_atom, std::string>,
                     caf::replies_to<ls_query_atom, std::string>::with<bool>>;
} // namespace shared
