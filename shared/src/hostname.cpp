#include <pl/os.hpp>

#if PL_OS == PL_OS_LINUX
#  include <limits.h>
#  include <unistd.h>
#endif

#include "hostname.hpp"

namespace shared {
std::string hostname() {
#if PL_OS == PL_OS_LINUX
  std::string buf(HOST_NAME_MAX + 1, '\0');
  const auto error_code = gethostname(buf.data(), HOST_NAME_MAX);

  if (error_code == -1)
    return "";

  const auto idx = buf.find_first_of('\0');
  buf.erase(idx);

  return buf;
#else
#  warning "shared::hostname: unsupported operating system"
  return "";
#endif
}
} // namespace shared
