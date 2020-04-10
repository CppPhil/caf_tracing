#include "caf/io/all.hpp"

#include "ip_address.hpp"

namespace shared {
caf::optional<std::string> ip_address() {
  const auto addresses = caf::io::network::interfaces::list_addresses(
    caf::io::network::protocol::ipv4, /* include_localhost */ false);

  if (addresses.empty())
    return caf::none;

  return addresses.front();
}
} // namespace shared
