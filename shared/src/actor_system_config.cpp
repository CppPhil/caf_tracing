#include "actor_system_config.hpp"
#include "client_actor_type.hpp"
#include "message.hpp"
#include "span_context.hpp"

namespace shared {
#define ADD_MSG_T(...) add_message_type<__VA_ARGS__>(#__VA_ARGS__)

actor_system_config::actor_system_config() {
  ADD_MSG_T(span_context);
  ADD_MSG_T(message<std::string>);
  ADD_MSG_T(message<std::string, client_actor_type>);
  ADD_MSG_T(message<std::vector<std::string>>);
  ADD_MSG_T(message<>);
}
} // namespace shared
