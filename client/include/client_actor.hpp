#pragma once
#include "caf/all.hpp"

#include "server_actor_type.hpp"

namespace e3::client {
/// The local client actor.
/// @param self A pointer to the actor
/// @param remote_actor The remote server actor to communicate with.
/// @return The behavior describing this actor.
shared::client_actor_type::behavior_type
client_actor(shared::client_actor_type::pointer self,
             const shared::server_actor_type& remote_actor);
} // namespace e3::client
