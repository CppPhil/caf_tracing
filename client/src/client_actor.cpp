#include <algorithm>
#include <string>
#include <utility>
#include <vector>

#include "aprint.hpp"
#include "atoms.hpp"
#include "client_actor.hpp"

namespace e3::client {
shared::client_actor_type::behavior_type
client_actor(shared::client_actor_type::pointer self,
             const shared::server_actor_type& remote_actor) {
  return {
    [self, remote_actor](caf::join_atom atom, std::string nickname) {
      // Delegate join message to the server.
      self->send(remote_actor, atom, std::move(nickname), self);
    },
    [self, remote_actor](shared::ls_atom atom) {
      // Pass the /ls request to the server and properly print the result.
      self->request(remote_actor, caf::infinite, atom)
        .then([self](const std::vector<std::string>& result) {
          shared::aprint(self, "Participants:\n[\n{}\n]\n",
                         caf::join(result, ",\n"));
        });
    },
    [self, remote_actor](caf::leave_atom atom, std::string goodbye_message) {
      // Delegate the quit message to the server.
      self->send(remote_actor, atom, std::move(goodbye_message));
    },
    [self, remote_actor](shared::local_chat_atom, std::string message) {
      // Send any chat messages stemming from the CLI to the server.
      self->send(remote_actor, shared::chat_atom::value, std::move(message));
    },
    [self](shared::chat_atom, const std::string& message) {
      // Print any chat messages received from the remote server actor.
      shared::aprint(self, message);
    },
    [self,
     remote_actor](shared::ls_query_atom,
                   const std::string& client_nickname) -> caf::result<bool> {
      // Send an /ls request to the server and return whether the resulting
      // client list contains this local client.
      // This is used to check whether our /quit message has arrived at the
      // server side when performing an orderly shutdown.
      auto response_promise = self->make_response_promise<bool>();

      self->request(remote_actor, caf::infinite, shared::ls_atom::value)
        .then([client_nickname, response_promise](
                const std::vector<std::string>& result) mutable {
          response_promise.deliver(
            std::find(result.begin(), result.end(), client_nickname)
            != result.end());
        });

      return response_promise;
    },
  };
}
} // namespace e3::client
