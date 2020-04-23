#pragma once
#include "span_context.hpp"

namespace shared {
std::unique_ptr<opentracing::Span>
create_span(const opentracing::Tracer* tracer,
            const span_context& serialized_span_context,
            const std::string& operation_name);
} // namespace shared
