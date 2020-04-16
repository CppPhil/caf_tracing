#include <algorithm>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

#include "aprint.hpp"
#include "atoms.hpp"
#include "client_actor.hpp"
#include "create_span.hpp"
#include "span_context.hpp"

namespace client {
shared::client_actor_type::behavior_type
client_actor(shared::client_actor_type::pointer self,
             const shared::server_actor_type& remote_actor) {
  return {
    [self, remote_actor](caf::join_atom atom, std::string nickname) {
      auto span = opentracing::Tracer::Global()->StartSpan("join (client)");
      span->SetTag("nickname", nickname);

      // Delegate join message to the server.
      self->send(
        remote_actor, atom, std::move(nickname), self,
        shared::span_context::inject(span).value_or((shared::span_context())));
    },
    [self, remote_actor](shared::ls_atom atom) {
      auto span = opentracing::Tracer::Global()->StartSpan("ls (client)");

      // Pass the /ls request to the server and properly print the result.
      self
        ->request(
          remote_actor, caf::infinite, atom,
          shared::span_context::inject(span).value_or((shared::span_context())))
        .then([self](const std::vector<std::string>& vector,
                     const shared::span_context& span_ctx) {
          auto span = shared::create_span(span_ctx, "ls recv (client)");

          const auto to_print = fmt::format("Participants:\n[\n{}\n]\n",
                                            caf::join(vector, ",\n"));

          span->SetTag("participants", to_print);

          shared::aprint(self, to_print);
        });
    },
    [self, remote_actor](caf::leave_atom atom, std::string goodbye_message) {
      auto span = opentracing::Tracer::Global()->StartSpan("quit (client)");
      span->SetTag("goodbye_message", goodbye_message);

      // Delegate the quit message to the server.
      self->send(
        remote_actor, atom, std::move(goodbye_message),
        shared::span_context::inject(span).value_or((shared::span_context())));
    },
    [self, remote_actor](shared::local_chat_atom, std::string message) {
      auto span = opentracing::Tracer::Global()->StartSpan("chat (client)");
      span->SetTag("message", message);

      // Send any chat messages stemming from the CLI to the server.
      self->send(
        remote_actor, shared::chat_atom::value, std::move(message),
        shared::span_context::inject(span).value_or((shared::span_context())));
    },
    [self](shared::chat_atom, const std::string& message,
           const shared::span_context& span_ctx) {
      auto span = shared::create_span(span_ctx, "chat recv (client)");
      span->SetTag("message", message);

      // Print any chat messages received from the remote server actor.
      shared::aprint(self, message);
    },
    [self,
     remote_actor](shared::ls_query_atom,
                   const std::string& client_nickname) -> caf::result<bool> {
      auto span = opentracing::Tracer::Global()->StartSpan("ls_query (client)");
      span->SetTag("client_nickname", client_nickname);

      // Send an /ls request to the server and return whether the resulting
      // client list contains this local client.
      // This is used to check whether our /quit message has arrived at the
      // server side when performing an orderly shutdown.
      auto response_promise = self->make_response_promise<bool>();

      self
        ->request(
          remote_actor, caf::infinite, shared::ls_atom::value,
          shared::span_context::inject(span).value_or((shared::span_context())))
        .then([client_nickname,
               response_promise](const std::vector<std::string>& result,
                                 const shared::span_context& span_ctx) mutable {
          auto span = shared::create_span(span_ctx, "ls query recv (client)");
          span->SetTag("client_nickname", client_nickname);
          span->SetTag("result", fmt::format("Participants:\n[\n{}\n]\n",
                                             caf::join(result, ",\n")));

          response_promise.deliver(
            std::find(result.begin(), result.end(), client_nickname)
            != result.end());
        });

      return response_promise;
    },
  };
}
} // namespace client
