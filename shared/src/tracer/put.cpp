#include <unordered_map>

#include <caf/all.hpp>

#include <fmt/format.h>

#include <jaegertracing/Tracer.h>

#include <pl/os.hpp>
#include <pl/thd/monitor.hpp>

#if PL_OS == PL_OS_LINUX
#  include <unistd.h>
#endif

#include "create_tracer.hpp"
#include "hostname.hpp"
#include "tracer/put.hpp"

namespace shared::tracer {
namespace {
int64_t pid() noexcept {
#if PL_OS == PL_OS_LINUX
  return getpid();
#else
#  warning "client/src/main.cpp:pid: Unsupported operating system."
  return 0;
#endif
}
} // namespace

pl::thd::monitor<
  std::unordered_map<caf::actor_id, std::shared_ptr<opentracing::Tracer>>>
  tracers({});

void put(caf::actor_id aid, std::string_view app) {
  tracers([aid, app](auto& map) {
    map.try_emplace(
      aid, create_tracer("config.yml",
                         fmt::format("caf_tracing-{}-{}-{} actor{}", app.data(),
                                     hostname(), pid(), aid)));
  });
}
} // namespace shared::tracer