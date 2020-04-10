#pragma once
#include <memory>
#include <string>

#include <opentracing/span.h>

#include "error.hpp"

namespace shared {
std::unique_ptr<opentracing::Span>
create_span(const std::string& serialized_span_context,
            const std::string& operation_name);
} // namespace shared
