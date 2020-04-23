#pragma once
#include <vector>

#include "caf/all.hpp"

#include "participant.hpp"
#include "server_actor_type.hpp"
#include "types.hpp"

namespace server {
/// The type of the state object of the chat server actor.
/// Contains the state of the chat server actor.
/// Consists of the participants of the chatroom managed by this chat server
/// actor.
struct server_chat_actor_state {
  std::vector<participant> participants;
};

/// The chat server actor.
/// @param self The chat server actor's self pointer.
/// @return The behavior describing this actor.
shared::server_actor_type::behavior_type chat_server(
  shared::server_actor_type::stateful_pointer<server_chat_actor_state> self);
} // namespace server
