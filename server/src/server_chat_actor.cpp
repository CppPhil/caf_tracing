#include <algorithm>
#include <functional>

#include <fmt/format.h>

#include "create_span.hpp"
#include "inject.hpp"
#include "server_chat_actor.hpp"
#include "shared/aprint.hpp"

namespace server {
namespace {
using self_pointer
  = shared::server_actor_type::stateful_pointer<server_chat_actor_state>;

/// Sends something to all participants matching the given filter.
/// @tparam Filter The type of `filter`.
/// @tparam Ts A template type parameter pack of the arguments to send.
/// @param self The server chat actor.
/// @param filter The filter to apply. The signature of the operator() must be
///               bool operator(const participant&)
/// @param xs The arguments to send to all participants satisfying the filter
///           predicate.
template <class Filter, class... Ts>
void send(self_pointer self, const Filter& filter, const Ts&... xs) {
  auto& participants = self->state.participants;

  for (const auto& participant : participants)
    if (filter(participant))
      self->send(participant.actor(), xs...);
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
void on_client_disconnect(
  self_pointer self,
  const caf::actor_addr& actor_address_of_disconnected_client,
  const std::string& goodbye_message,
  const std::string& span_context) noexcept {
  auto span = shared::create_span(span_context, "quit (server)");

  span->SetTag("goodbye_message", goodbye_message);

  const auto inject_res = shared::inject(span->context());

  if (!inject_res.has_value())
    shared::aprint(self, "Couldn't inject span context for quit: \"{}\"\n",
                   inject_res.error());

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
      self, [](const participant&) { return true; }, shared::chat_atom::value,
      [&goodbye_message, &nickname] {
        if (goodbye_message.empty())
          return fmt::format("{} has left the chatroom.\n", nickname);

        return fmt::format("{} has left the chatroom: \"{}\"\n", nickname,
                           goodbye_message);
      }(),
      inject_res.value_or(""));
  }
}

/// Handles client connection.
/// @param self The server chat actor.
/// @param nickname The nickname of the client trying to connect.
/// @param client_actor The client actor.
/// @param span_context The span context.
void on_client_connect(self_pointer self, const std::string& nickname,
                       const shared::client_actor_type& client_actor,
                       const std::string& span_context) {
  auto span = shared::create_span(span_context, "join (server)");

  span->SetTag("nickname", nickname);

  const auto inject_res = shared::inject(span->context());

  if (!inject_res.has_value())
    shared::aprint(self, "Couldn't inject span context for join: \"{}\"\n",
                   inject_res.error());
  const inject_result = inject_res.value_or("");

  auto& participants = self->state.participants;

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
    self,
    [&client_actor](const participant& participant) {
      return participant.actor() != client_actor;
    },
    shared::chat_atom::value,
    fmt::format("{} has joined the chatroom.\n", nickname), inject_result);
}

/// Handles chat messages originating from clients.
/// @param self This server chat actor.
/// @param message The message received from a client.
/// @param span_context The span context.
void on_chat(self_pointer self, const std::string& message,
             const std::string& span_context) {
  auto span = shared::create_span(span_context, "chat (server)");

  span->SetTag("message", message);

  const auto inject_res = shared::inject(span->context());

  if (!inject_res.has_value())
    shared::aprint(self, "Couldn't inject span context for chat: \"{}\"\n",
                   inject_res.error());

  auto& sender = self->current_sender();
  auto& participants = self->state.participants;

  // Find the client that sent the message by its actor address.
  const auto it = std::find(participants.begin(), participants.end(),
                            sender->address());

  if (it != participants.end()) { // If it was found
    // Send the chat message to all other chat participants.
    send(
      self,
      [&sender](const participant& participant) {
        return participant.actor() != sender;
      },
      shared::chat_atom::value,
      fmt::format("{}: \"{}\"\n", it->nickname(), message),
      inject_res.value_or(""));
  }
}

/// Handles /ls requests.
/// @param self This server chat actor.
/// @param span_context The span context.
/// @return A vector of the nicknames of the chat participants and the span
/// context.
std::pair<std::vector<std::string>, std::string>
on_ls(self_pointer self, const std::string& span_context) {
  auto span = shared::create_span(span_context, "ls (server)");

  const auto inject_res = shared::inject(span->context());

  if (!inject_res.has_value())
    shared::aprint(self, "Couldn't inject span context for ls: \"{}\"\n",
                   inject_res.error());

  auto& participants = self->state.participants;

  // Allocate enough storage for the nicknames.
  std::vector<std::string> nicknames(participants.size(), "");

  // Map the nicknames into the vector.
  std::transform(participants.begin(), participants.end(), nicknames.begin(),
                 std::mem_fn(&participant::nickname));

  return std::make_pair(std::move(nicknames), inject_res.value_or(""));
}
} // namespace

shared::server_actor_type::behavior_type chat_server(self_pointer self) {
  return {
    [self](caf::join_atom, const std::string& nickname,
           const shared::client_actor_type& client_actor,
           const std::string& span_context) {
      on_client_connect(self, nickname, client_actor, span_context);
    },
    [self](shared::chat_atom, const std::string& message,
           const std::string& span_context) {
      on_chat(self, message, span_context);
    },
    [self](caf::leave_atom, const std::string& goodbye_message,
           const std::string& span_context) {
      on_client_disconnect(self, self->current_sender()->address(),
                           goodbye_message, span_context);
    },
    [self](shared::ls_atom, const std::string& span_context) {
      return on_ls(self, span_context);
    },
  };
}
} // namespace server
