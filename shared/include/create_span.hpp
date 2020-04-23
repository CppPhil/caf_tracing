#pragma once
#include "span_context.hpp"

namespace shared {
std::unique_ptr<opentracing::Span>
create_span(const opentracing::Tracer* tracer, const caf::tracing_data* data,
            const std::string& operation_name);
} // namespace shared
