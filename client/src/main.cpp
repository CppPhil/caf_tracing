#include <cstdint>

#include <iostream>
#include <string>

#include "fmt/format.h"

#include "caf/io/all.hpp"

#include "atoms.hpp"
#include "client_actor.hpp"
#include "shutdown.hpp"

namespace {
struct config : caf::actor_system_config {
  std::string host;
  uint16_t port = 0;

  config() {
    add_message_type<e3::shared::client_actor_type>("client_actor_type");

    opt_group(custom_options_, "global")
      .add(host, "host,H", "server host")
      .add(port, "port,p", "port");
  }

  std::string help_text() const {
    return custom_options_.help_text();
  }
};

void run_client(caf::actor_system& system, const config& config) noexcept {
  const auto expected_remote_actor
    = caf::io::remote_actor<e3::shared::server_actor_type>(system, config.host,
                                                           config.port);

  if (!expected_remote_actor) {
    fmt::print(stderr, "Could not connect to remote actor!\n");
    return;
  }

  const auto& remote_actor = *expected_remote_actor;

  const auto client_actor = system.spawn(&e3::client::client_actor,
                                         remote_actor);

  caf::scoped_actor self(system);

  std::string username;
  fmt::print("Enter username: ");
  std::getline(std::cin, username);

  // Connect to the remote server.
  self->send(client_actor, caf::join_atom::value, username);

  const auto invalid_command = [](const std::string& line) {
    fmt::print(stderr, "Invalid command: \"{}\"\n", line);
  };

  // Enter the CLI loop
  for (std::string line_buffer; std::getline(std::cin, line_buffer);) {
    std::vector<caf::string_view> parts;
    caf::split(parts, line_buffer, ' ');

    if (parts.empty())
      invalid_command(line_buffer); // NOLINT(bugprone-branch-clone)
    else if (parts.front() == "/ls")
      self->send(client_actor, e3::shared::ls_atom::value);
    else if (parts.front() == "/quit") {
      self->send(client_actor, caf::leave_atom::value,
                 caf::join(++(parts.begin()), parts.end(), " "));
      e3::client::shutdown(system, self, client_actor, username);
      return;
    } else if (!parts.front().empty()
               && parts.front().front() != '/') // Handle chat command
      self->send(client_actor, e3::shared::local_chat_atom::value, line_buffer);
    else
      invalid_command(line_buffer);
  }
}
} // namespace

void caf_main(caf::actor_system& system, const config& config) {
  if (config.host.empty()) {
    fmt::print(stderr, "host option was not set!\n\n{}", config.help_text());
    return;
  }

  if (config.port == 0) {
    fmt::print(stderr, "port option was not set!\n\n{}", config.help_text());
    return;
  }

  run_client(system, config);
}

CAF_MAIN(caf::io::middleman)