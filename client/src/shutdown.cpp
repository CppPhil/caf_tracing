#include "shutdown.hpp"
#include "aprint.hpp"
#include "atoms.hpp"

namespace client {
void shutdown(caf::actor_system& system, caf::scoped_actor& self,
              const shared::client_actor_type& client_actor,
              const std::string& client_nickname) noexcept {
  // Spin until the server has received our /quit message.
  // -> Then exit.
  for (auto keep_spinning = true; keep_spinning;) {
    self
      ->request(client_actor, caf::infinite, shared::ls_query_atom::value,
                client_nickname)
      .receive(
        [&self, &client_actor,
         &keep_spinning](bool is_client_still_registered) {
          if (!is_client_still_registered) {
            self->send_exit(client_actor, caf::exit_reason::user_shutdown);
            keep_spinning = false;
          }
        },
        [self = self.ptr(), &system, &client_actor,
         &keep_spinning](const caf::error& error) {
          shared::aprint(self, "Unexpected error during shutdown: \"{}\"\n",
                         system.render(error));
          self->send_exit(client_actor, caf::exit_reason::kill);
          keep_spinning = false;
        });
  }
}
} // namespace client
