#pragma once
#include <string>

#include "caf/fwd.hpp"

#include "types.hpp"

namespace client {
/// Routine to perform an orderly shutdown of the client application.
/// @param system The actor system.
/// @param self THe scoped actor from main.cpp
/// @param client_actor The local client actor.
/// @param client_nickname The nickname of this client.
void shutdown(caf::actor_system& system, caf::scoped_actor& self,
              const shared::client_actor_type& client_actor,
              const std::string& client_nickname) noexcept;
} // namespace client
