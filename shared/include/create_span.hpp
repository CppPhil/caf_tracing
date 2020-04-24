#pragma once
#include <memory>
#include <string>

#include <caf/all.hpp>

#include <opentracing/span.h>
#include <opentracing/tracer.h>

namespace shared {
std::unique_ptr<opentracing::Span>
create_span(const opentracing::Tracer* tracer, const caf::tracing_data* data,
            const std::string& operation_name);
} // namespace shared
