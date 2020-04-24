#include <algorithm>

#include "actor_profiler.hpp"
#include "aprint.hpp"
#include "client_actor.hpp"
#include "create_span.hpp"
#include "get_tracing_data.hpp"
#include "span_context.hpp"
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
      auto span = shared::create_span(shared::tracer::get(self->id()),
                                      shared::get_tracing_data(self), "join");

      span->SetTag("nickname", nickname);

      shared::set_span_context(
        shared::span_context::inject(shared::tracer::get(self->id()), span)
          .value_or(shared::span_context{})
          .str());

      // Delegate join message to the server.
      self->send(remote_actor, atom, std::move(nickname), self);
    },
    [self, remote_actor](shared::ls_atom atom) {
      auto span = shared::create_span(shared::tracer::get(self->id()),
                                      shared::get_tracing_data(self), "ls");

      shared::set_span_context(
        shared::span_context::inject(shared::tracer::get(self->id()), span)
          .value_or(shared::span_context{})
          .str());

      // Pass the /ls request to the server and properly print the result.
      self->request(remote_actor, caf::infinite, atom)
        .then([self](const std::vector<std::string>& vector) {
          auto span = shared::create_span(shared::tracer::get(self->id()),
                                          shared::get_tracing_data(self),
                                          "ls result");

          const auto to_print = fmt::format("Participants:\n[\n{}\n]\n",
                                            caf::join(vector, ",\n"));

          span->SetTag("participants", to_print);

          shared::aprint(self, to_print);
        });
    },
    [self, remote_actor](caf::leave_atom atom, std::string goodbye_message) {
      auto span = shared::create_span(shared::tracer::get(self->id()),
                                      shared::get_tracing_data(self), "quit");

      span->SetTag("goodbye_message", goodbye_message);

      shared::set_span_context(
        shared::span_context::inject(shared::tracer::get(self->id()), span)
          .value_or(shared::span_context{})
          .str());

      // Delegate the quit message to the server.
      self->send(remote_actor, atom, std::move(goodbye_message));
    },
    [self, remote_actor](shared::local_chat_atom, std::string message) {
      auto span = shared::create_span(shared::tracer::get(self->id()),
                                      shared::get_tracing_data(self),
                                      "local_chat");

      span->SetTag("message", message);

      shared::set_span_context(
        shared::span_context::inject(shared::tracer::get(self->id()), span)
          .value_or(shared::span_context{})
          .str());

      // Send any chat messages stemming from the CLI to the server.
      self->send(remote_actor, shared::chat_atom_v, std::move(message));
    },
    [self](shared::chat_atom, const std::string& message) {
      auto span = shared::create_span(shared::tracer::get(self->id()),
                                      shared::get_tracing_data(self),
                                      "receive chat");

      span->SetTag("message", message);

      // Print any chat messages received from the remote server actor.
      shared::aprint(self, message);
    },
    [self,
     remote_actor](shared::ls_query_atom,
                   const std::string& client_nickname) -> caf::result<bool> {
      const auto* tracer = shared::tracer::get(self->id());
      auto* trcing_data = shared::get_tracing_data(self);
      auto span = shared::create_span(tracer, trcing_data, "ls_query");

      span->SetTag("client_nickname", client_nickname);

      // Send an /ls request to the server and return whether the resulting
      // client list contains this local client.
      // This is used to check whether our /quit message has arrived at the
      // server side when performing an orderly shutdown.
      auto response_promise = self->make_response_promise<bool>();

      shared::set_span_context(shared::span_context::inject(tracer, span)
                                 .value_or(shared::span_context{})
                                 .str());

      self->request(remote_actor, caf::infinite, shared::ls_atom_v)
        .then([tracer, trcing_data, client_nickname, response_promise](
                const std::vector<std::string>& result) mutable {
          auto span = shared::create_span(tracer, trcing_data,
                                          "receive ls_query result");
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
