#pragma once
#include <memory>
#include <string>

#include <opentracing/tracer.h>

#include "error.hpp"

namespace shared {
tl::expected<std::unique_ptr<opentracing::SpanContext>, error>
extract(const std::string& string);
} // namespace shared
