#include "fmt/format.h"

#include "caf/io/all.hpp"

#include "ip_address.hpp"
#include "server_chat_actor.hpp"

namespace {
struct config : caf::actor_system_config {
  std::string ip_address;
  uint16_t port = 0;

  config() : ip_address(e3::shared::ip_address().value_or("localhost")) {
    add_message_type<e3::shared::client_actor_type>("client_actor_type");

    opt_group(custom_options_, "global").add(port, "port,p", "port");
  }

  std::string help_text() const {
    return custom_options_.help_text();
  }
};

void run_server(caf::actor_system& system, const config& config) noexcept {
  const auto actor = system.spawn(&e3::server::chat_server);

  const auto expected_port = caf::io::publish(actor, config.port,
                                              config.ip_address.c_str());

  if (expected_port) {
    const auto port = *expected_port;
    fmt::print(
      "published chat server actor at ip_address \"{}\", port: \"{}\"\n",
      config.ip_address, port);
  } else
    fmt::print(stderr, "error: {}\n", system.render(expected_port.error()));
}
} // namespace

void caf_main(caf::actor_system& system, const config& config) {
  if (config.port == 0) {
    fmt::print(stderr, "port option was not set!\n\n{}", config.help_text());
    return;
  }

  run_server(system, config);
}

CAF_MAIN(caf::io::middleman)
