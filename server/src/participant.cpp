#include <utility>

#include "participant.hpp"

namespace server {
participant::participant(shared::client_actor_type actor, std::string nickname)
  : actor_(std::move(actor)), nickname_(std::move(nickname)) {
  // nop
}

const shared::client_actor_type& participant::actor() const noexcept {
  return actor_;
}

const std::string& participant::nickname() const noexcept {
  return nickname_;
}

bool operator==(const participant& lhs, const caf::actor_addr& rhs) noexcept {
  return lhs.actor()->address() == rhs;
}

bool operator==(const caf::actor_addr& lhs, const participant& rhs) noexcept {
  return rhs == lhs;
}

bool operator!=(const participant& lhs, const caf::actor_addr& rhs) noexcept {
  return !(lhs == rhs);
}

bool operator!=(const caf::actor_addr& lhs, const participant& rhs) noexcept {
  return rhs != lhs;
}

bool operator==(const participant& lhs, const participant& rhs) noexcept {
  return lhs == rhs.actor()->address();
}

bool operator!=(const participant& lhs, const participant& rhs) noexcept {
  return !(lhs == rhs);
}
} // namespace server
