#include "actor_system_config.hpp"

namespace shared {
actor_system_config::actor_system_config() {
  profiler = &p;
  tracing_context = &tracing_data_fac;
}
} // namespace shared
