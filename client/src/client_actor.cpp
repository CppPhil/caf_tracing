#include <algorithm>

#include "aprint.hpp"
#include "client_actor.hpp"
#include "create_span.hpp"
#include "tracer/get.hpp"
#include "tracer/put.hpp"
#include "types.hpp"

namespace client {
shared::client_actor_type::behavior_type
client_actor(shared::client_actor_type::pointer self,
             const shared::server_actor_type& remote_actor) {
  shared::tracer::put(self->id(), "client");

  return {
    [self, remote_actor](caf::join_atom atom, std::string nickname) {
      //      shared::tracing_sender self{self_, "join (client)"};
      //      self.SetTag("nickname", nickname);

      // Delegate join message to the server.
      self->send(remote_actor, atom, std::move(nickname), self);
    },
    [self, remote_actor](shared::ls_atom atom) {
      // shared::tracing_sender self{self_, "ls (client)"};

      // Pass the /ls request to the server and properly print the result.
      self->request(remote_actor, caf::infinite, atom)
        .then([self](const std::vector<std::string>& vector) {
          /*auto span = shared::create_span(message.span_ctx(),
                                          "ls recv (client)");
*/
          const auto to_print = fmt::format("Participants:\n[\n{}\n]\n",
                                            caf::join(vector, ",\n"));

          // span->SetTag("participants", to_print);

          shared::aprint(self, to_print);
        });
    },
    [self, remote_actor](caf::leave_atom atom, std::string goodbye_message) {
      // shared::tracing_sender self{self_, "quit (client)"};
      // self.SetTag("goodbye_message", goodbye_message);

      // Delegate the quit message to the server.
      self->send(remote_actor, atom, std::move(goodbye_message));
    },
    [self, remote_actor](shared::local_chat_atom, std::string message) {
      // shared::tracing_sender self{self_, "chat (client)"};
      // self.SetTag("message", message);

      // Send any chat messages stemming from the CLI to the server.
      self->send(remote_actor, shared::chat_atom_v, std::move(message));
    },
    [self](shared::chat_atom, const std::string& message) {
      // auto span = shared::create_span(msg.span_ctx(), "chat recv (client)");
      // span->SetTag("message", message);

      // Print any chat messages received from the remote server actor.
      shared::aprint(self, message);
    },
    [self,
     remote_actor](shared::ls_query_atom,
                   const std::string& client_nickname) -> caf::result<bool> {
      // shared::tracing_sender self{self_, "ls_query (client)"};
      // self.SetTag("client_nickname", client_nickname);

      // Send an /ls request to the server and return whether the resulting
      // client list contains this local client.
      // This is used to check whether our /quit message has arrived at the
      // server side when performing an orderly shutdown.
      auto response_promise = self->make_response_promise<bool>();

      self->request(remote_actor, caf::infinite, shared::ls_atom_v)
        .then([client_nickname, response_promise](
                const std::vector<std::string>& result) mutable {
          /*auto span = shared::create_span(message.span_ctx(),
                                          "ls query recv (client)");
          span->SetTag("client_nickname", client_nickname);
          span->SetTag("result", fmt::format("Participants:\n[\n{}\n]\n",
                                             caf::join(result, ",\n")));*/

          response_promise.deliver(
            std::find(result.begin(), result.end(), client_nickname)
            != result.end());
        });

      return response_promise;
    },
  };
}
} // namespace client
