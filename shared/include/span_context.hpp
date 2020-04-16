#pragma once
#include <string>

#include <opentracing/tracer.h>

#include "error.hpp"

namespace shared {
class span_context {
public:
  static tl::expected<span_context, error>
  inject(const opentracing::SpanContext& span_ctx);

  static tl::expected<span_context, error>
  inject(const opentracing::Span& span);

  tl::expected<std::unique_ptr<opentracing::SpanContext>, error>
  extract() const;

private:
  explicit span_context(std::string&& serialized_span_context);

  std::string buffer_;
};
} // namespace shared
