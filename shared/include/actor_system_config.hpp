#pragma once
#include <caf/all.hpp>

#include "profiler.hpp"
#include "tracing_data_factory.hpp"

namespace shared {
struct actor_system_config : caf::actor_system_config {
  profiler p;
  tracing_data_factory tracing_data_fac;

  actor_system_config();
};
} // namespace shared
