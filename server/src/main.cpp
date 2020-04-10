#include <cstdio>

#include <gsl/gsl_util>

#include <fmt/format.h>

#include "caf/io/all.hpp"

#include "ip_address.hpp"
#include "server_chat_actor.hpp"
#include "setup_tracer.hpp"

namespace {
struct config : caf::actor_system_config {
  std::string ip_address;
  uint16_t port = 0;

  config() : ip_address(shared::ip_address().value_or("localhost")) {
    add_message_type<shared::client_actor_type>("client_actor_type");

    opt_group(custom_options_, "global").add(port, "port,p", "port");
  }

  std::string help_text() const {
    return custom_options_.help_text();
  }
};

void run_server(caf::actor_system& system, const config& config) noexcept {
  const auto actor = system.spawn(&server::chat_server);

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

int main(int argc, char** argv) {
  if (argc < 2) {
    fprintf(stderr,
            "No YAML config file was passed as a command line argument!\n");
    return 1;
  }

  shared::setup_tracer(argv[1], "caf_tracing-server");

  static std::vector<char*> args;

  for (auto i = 0; i < argc; ++i) {
    if (i == 1)
      continue;

    const auto len = strlen(argv[i]);
    auto* p = new char[len];
    memcpy(p, argv[i], len);
    args.push_back(p);
  }

  [[maybe_unused]] auto final_act = gsl::finally([] {
    for (auto* p : args)
      delete[] p;
  });

  return caf::exec_main<caf::io::middleman>(caf_main, argc - 1, args.data());
}
