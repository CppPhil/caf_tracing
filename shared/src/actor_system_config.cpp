#include "actor_system_config.hpp"
#include "message.hpp"
#include "span_context.hpp"

namespace shared {
actor_system_config::actor_system_config() {
  add_message_type<span_context>("span_context");
  // TODO: Put stuff here.
}
} // namespace shared
