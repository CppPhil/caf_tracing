#pragma once 
#include <opentracing/tracer.h>

#include "error.hpp"

namespace shared{
tl::expected<std::string, error> inject(const opentracing::SpanContext& sc);
} // namespace shared

