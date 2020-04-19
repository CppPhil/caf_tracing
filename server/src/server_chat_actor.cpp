#include <algorithm>
#include <functional>
#include <tuple>

#include <fmt/format.h>

#include <tl/optional.hpp>

#include <pl/meta/remove_cvref.hpp>

#include "aprint.hpp"
#include "create_span.hpp"
#include "server_chat_actor.hpp"
#include "span_context.hpp"
#include "tracing_sender.hpp"

namespace server {
namespace {
using self_pointer
  = shared::server_actor_type::stateful_pointer<server_chat_actor_state>;

template <size_t Idx>
struct arg {
  static_assert(Idx <= 9, "Idx is too large!");

  static constexpr char str[] = {'A', 'r', 'g', 'u',       'm', 'e',
                                 'n', 't', ' ', Idx + '0', '\0'};
};

template <class T>
opentracing::Value make_value(const T& x) {
  using Stripped = pl::meta::remove_cvref_t<T>;

  if constexpr (caf::is_atom_constant<Stripped>::value) {
    return "caf::atom_constant<" + caf::to_string(x) + ">";
  } else {
    return x;
  }
}

template <class TracingSender, size_t... Ints, class... Ts>
void log_args_impl(TracingSender& tracing_sender, std::index_sequence<Ints...>,
                   const Ts&... xs) {
  tracing_sender.Log({std::make_pair(opentracing::string_view(arg<Ints>::str),
                                     make_value(xs))...});
}

template <class TracingSender, class... Ts>
void log_args(TracingSender& tracing_sender, const Ts&... xs) {
  log_args_impl(tracing_sender, (std::index_sequence_for<Ts...>()), xs...);
}

/// Sends something to all participants matching the given filter.
/// @tparam Filter The type of `filter`.
/// @tparam Ts A template type parameter pack of the arguments to send.
/// @param operation_name The operation name to use for the tracing Span.
/// @param parent_span The (optional) parent Span to use.
/// @param self_ The server chat actor.
/// @param filter The filter to apply. The signature of the operator() must be
///               bool operator(const participant&)
/// @param xs The arguments to send to all participants satisfying the filter
///           predicate.
template <class Filter, class... Ts>
void send(opentracing::string_view operation_name,
          tl::optional<shared::span_context> parent_span, self_pointer self_,
          const Filter& filter, const Ts&... xs) {
  auto& participants = self_->state.participants;

  shared::tracing_sender self{self_, operation_name, parent_span};

  log_args(self, xs...);

  for (const auto& participant : participants)
    if (filter(participant))
      self.send(participant.actor(), xs...);
}

/// Handles client disconnect.
/// Used for orderly shutdown through a /quit message optionally containing a
/// goodbye message as well as for unorderly shutdown (which never contains a
/// goodbye message) through a down_msg when a client crashes or receives SIGINT
/// through CTRL + C etc.
/// @param self The server chat actor.
/// @param actor_address_of_disconnected_client The actor address of the client
///                                             to disconnect.
/// @param goodbye_message The goodbye message of the client
///                        (possibly empty).
/// @param span_ctx The span context.
void on_client_disconnect(
  self_pointer self,
  const caf::actor_addr& actor_address_of_disconnected_client,
  const std::string& goodbye_message,
  const shared::span_context& span_ctx) noexcept {
  auto span = shared::create_span(span_ctx, "quit (server)");
  span->SetTag("goodbye_message", goodbye_message);

  auto& participants = self->state.participants;

  // Find the participant by its actor address.
  const auto it = std::find(participants.begin(), participants.end(),
                            actor_address_of_disconnected_client);

  if (it != participants.end()) { // if it was found
    const auto nickname = it->nickname();

    // Remove it.
    participants.erase(it);

    // Send all clients an appropriate chat message containing the goodbye
    // message if there's one.
    send(
      "server: send chat message (disconnect)",
      tl::make_optional(
        shared::span_context::inject(span).value_or((shared::span_context()))),
      self, [](const participant&) { return true; }, shared::chat_atom::value,
      [&goodbye_message, &nickname] {
        if (goodbye_message.empty())
          return fmt::format("{} has left the chatroom.\n", nickname);

        return fmt::format("{} has left the chatroom: \"{}\"\n", nickname,
                           goodbye_message);
      }());
  }
}

/// Handles client connection.
/// @param self The server chat actor.
/// @param nickname The nickname of the client trying to connect.
/// @param client_actor The client actor.
/// @param span_ctx The span context.
void on_client_connect(self_pointer self, const std::string& nickname,
                       const shared::client_actor_type& client_actor,
                       const shared::span_context& span_ctx) {
  auto span = shared::create_span(span_ctx, "join (server)");
  span->SetTag("nickname", nickname);
  auto& participants = self->state.participants;

  const auto inject_result
    = shared::span_context::inject(span).value_or((shared::span_context()));

  // Monitor the client so that we receive down_msgs if it crashes.
  self->monitor(client_actor);

  // Register a down handler to disconnect clients by their actor addresses when
  // they crash.
  self->set_down_handler(
    [self, inject_result](caf::scheduled_actor*,
                          const caf::down_msg& down_message) {
      on_client_disconnect(self, down_message.source, "", inject_result);
    });

  // Add the participant.
  participants.emplace_back(client_actor, nickname);

  // Inform all other participants about the new client.
  send(
    "server: send chat message (connect)", tl::make_optional(inject_result),
    self,
    [&client_actor](const participant& participant) {
      return participant.actor() != client_actor;
    },
    shared::chat_atom::value,
    fmt::format("{} has joined the chatroom.\n", nickname));
}

/// Handles chat messages originating from clients.
/// @param self This server chat actor.
/// @param message The message received from a client.
/// @param span_ctx The span context.
void on_chat(self_pointer self, const std::string& message,
             const shared::span_context& span_ctx) {
  auto span = shared::create_span(span_ctx, "chat (server)");
  span->SetTag("message", message);
  auto& sender = self->current_sender();
  auto& participants = self->state.participants;

  // Find the client that sent the message by its actor address.
  const auto it = std::find(participants.begin(), participants.end(),
                            sender->address());

  if (it != participants.end()) { // If it was found
    // Send the chat message to all other chat participants.
    send(
      "server: send chat message (chat)",
      tl::make_optional(
        shared::span_context::inject(span).value_or((shared::span_context()))),
      self,
      [&sender](const participant& participant) {
        return participant.actor() != sender;
      },
      shared::chat_atom::value,
      fmt::format("{}: \"{}\"\n", it->nickname(), message));
  }
}

/// Handles /ls requests.
/// @param self This server chat actor.
/// @param span_ctx The span context.
/// @return A vector of the nicknames of the chat participants and the span
///         context.
shared::message<std::vector<std::string>>
on_ls(self_pointer self, const shared::span_context& span_ctx) {
  auto span = shared::create_span(span_ctx, "ls (server)");
  auto& participants = self->state.participants;

  // Allocate enough storage for the nicknames.
  std::vector<std::string> nicknames(participants.size(), "");

  // Map the nicknames into the vector.
  std::transform(participants.begin(), participants.end(), nicknames.begin(),
                 std::mem_fn(&participant::nickname));

  span->SetTag("nicknames", caf::join(nicknames, ", "));

  return shared::message(
    shared::span_context::inject(span).value_or((shared::span_context())),
    std::move(nicknames));
}
} // namespace

shared::server_actor_type::behavior_type chat_server(self_pointer self) {
  return {
    [self](
      caf::join_atom,
      const shared::message<std::string, shared::client_actor_type>& message) {
      const auto& [nickname, client_actor] = message.tuple();
      on_client_connect(self, nickname, client_actor, message.span_ctx());
    },
    [self](shared::chat_atom, const shared::message<std::string>& msg) {
      const auto& [message] = msg.tuple();
      on_chat(self, message, msg.span_ctx());
    },
    [self](caf::leave_atom, const shared::message<std::string>& message) {
      const auto& [goodbye_message] = message.tuple();

      on_client_disconnect(self, self->current_sender()->address(),
                           goodbye_message, message.span_ctx());
    },
    [self](shared::ls_atom, const shared::message<>& message) {
      return on_ls(self, message.span_ctx());
    },
  };
}
} // namespace server
