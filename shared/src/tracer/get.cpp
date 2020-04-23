#include <unordered_map>

#include <pl/thd/monitor.hpp>

#include "tracer/get.hpp"

namespace shared::tracer {
extern pl::thd::monitor<
  std::unordered_map<caf::actor_id, std::shared_ptr<opentracing::Tracer>>>
  tracers;

const opentracing::Tracer* get(caf::actor_id aid) {
  return tracers([aid](const auto& map) {
    auto it = map.find(aid);

    if (it == map.end())
      return static_cast<const opentracing::Tracer*>(nullptr);

    const auto& [actor_id, tracer_shared_ptr] = *it;
    (void) actor_id;

    return static_cast<const opentracing::Tracer*>(tracer_shared_ptr.get());
  });
}
} // namespace shared::tracer
