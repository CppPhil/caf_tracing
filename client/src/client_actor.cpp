#include <algorithm>
#include <string>
#include <utility>
#include <vector>

#include <fmt/ostream.h>

#include <opentracing/tracer.h>

#include "aprint.hpp"
#include "atoms.hpp"
#include "client_actor.hpp"
#include "create_span.hpp"
#include "inject.hpp"

namespace client {
shared::client_actor_type::behavior_type
client_actor(shared::client_actor_type::pointer self,
             const shared::server_actor_type& remote_actor) {
  return {
    [self, remote_actor](caf::join_atom atom, std::string nickname) {
      auto span = opentracing::Tracer::Global()->StartSpan("join (client)");

      span->SetTag("nickname", nickname);

      const auto inject_res = shared::inject(span->context());

      if (!inject_res.has_value())
        shared::aprint(self, "Couldn't inject span context for join: \"{}\"\n",
                       inject_res.error());

      // Delegate join message to the server.
      self->send(remote_actor, atom, std::move(nickname), self,
                 inject_res.value_or(""));
    },
    [self, remote_actor](shared::ls_atom atom) {
      auto span = opentracing::Tracer::Global()->StartSpan("ls (client)");

      const auto inject_res = shared::inject(span->context());

      if (!inject_res.has_value())
        shared::aprint(self, "Couldn't inject span context for ls \"{}\"\n",
                       inject_res.error());

      // Pass the /ls request to the server and properly print the result.
      self->request(remote_actor, caf::infinite, atom, inject_res.value_or(""))
        .then(
          [self](
            const std::pair<std::vector<std::string>, std::string>& result) {
            const auto& [vector, span_context_str] = *result;

            auto span = shared::create_span(span_context_str);

            const auto to_print = fmt::format("Participants:\n[\n{}\n]\n",
                                              caf::join(vector, ",\n"));

            span->SetTag("participants", to_print);

            shared::aprint(self, to_print);
          });
    },
    [self, remote_actor](caf::leave_atom atom, std::string goodbye_message) {
      auto span = opentracing::Tracer::Global()->StartSpan("quit (client)");

      span->SetTag("goodbye_message", goodbye_message);

      const auto inject_res = shared::inject(span->context());

      if (!inject_res.has_value())
        shared::aprint(self, "Couldn't inject span context for quit \"{}\"\n",
                       inject_res.error());

      // Delegate the quit message to the server.
      self->send(remote_actor, atom, std::move(goodbye_message),
                 inject_res.value_or(""));
    },
    [self, remote_actor](shared::local_chat_atom, std::string message) {
      auto span = opentracing::Tracer::Global()->StartSpan("chat (client)");

      span->SetTag("message", message);

      const auto inject_res = shared::inject(span->context());

      if (!inject_res.has_value())
        shared::aprint(self, "Couldn't inject span context for chat \"{}\"\n",
                       inject_res.error());

      // Send any chat messages stemming from the CLI to the server.
      self->send(remote_actor, shared::chat_atom::value, std::move(message),
                 inject_res.value_or(""));
    },
    [self](shared::chat_atom, const std::string& message,
           const std::string& span_context) {
      auto span = shared::create_span(span_context, "chat recv (client)");

      span->SetTag("message", message);

      // Print any chat messages received from the remote server actor.
      shared::aprint(self, message);
    },
    [self,
     remote_actor](shared::ls_query_atom,
                   const std::string& client_nickname) -> caf::result<bool> {
      auto span = opentracing::Tracer::Global()->StartSpan("ls_query (client)");

      span->SetTag("client_nickname", client_nickname);

      const auto inject_res = shared::inject(span->context());

      if (!inject_res.has_value())
        shared::aprint(self,
                       "Couldn't inject span context for ls_query: \"{}\"\n",
                       inject_res.error());

      // Send an /ls request to the server and return whether the resulting
      // client list contains this local client.
      // This is used to check whether our /quit message has arrived at the
      // server side when performing an orderly shutdown.
      auto response_promise = self->make_response_promise<bool>();

      self
        ->request(remote_actor, caf::infinite, shared::ls_atom::value,
                  inject_res.value_or(""))
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
} // namespace client
