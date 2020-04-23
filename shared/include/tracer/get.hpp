#pragma once
#include <opentracing/tracer.h>

#include <caf/all.hpp>

namespace shared::tracer {
const opentracing::Tracer* get(caf::actor_id aid);
} // namespace shared::tracer
