#pragma once
#include <string>

#include "caf/all.hpp"

#include "types.hpp"

namespace server {
/// Type to represent a chat participant (client).
/// A chat participant (client).
/// Associates the remote client actor with the nickname of the client.
class participant {
public:
  /// Create a participant.
  /// @param actor The remote client.
  /// @param nickname The nickname of the remote client.
  participant(shared::client_actor_type actor, std::string nickname);

  /// Read accessor for the remote client actor.
  /// @return A reference to the remote client actor.
  [[nodiscard]] const shared::client_actor_type& actor() const noexcept;

  /// Read accessor for the client's nickname.
  /// @return A reference to the client's nickname.
  [[nodiscard]] const std::string& nickname() const noexcept;

private:
  shared::client_actor_type actor_;
  std::string nickname_;
};

bool operator==(const participant& lhs, const caf::actor_addr& rhs) noexcept;

bool operator==(const caf::actor_addr& lhs, const participant& rhs) noexcept;

bool operator!=(const participant& lhs, const caf::actor_addr& rhs) noexcept;

bool operator!=(const caf::actor_addr& lhs, const participant& rhs) noexcept;

bool operator==(const participant& lhs, const participant& rhs) noexcept;

bool operator!=(const participant& lhs, const participant& rhs) noexcept;
} // namespace server
