#include <cstdio>

#include <fmt/format.h>

#include "create_span.hpp"

namespace shared {
std::unique_ptr<opentracing::Span>
create_span(const span_context& serialized_span_context,
            const std::string& operation_name) {
  const auto ctx = span_context.extract();

  if (!ctx.has_value() || *ctx == nullptr) {
    fmt::print(stderr, "Span \"{}\" has no parent!\n", operation_name);
    return opentracing::Tracer::Global()->StartSpan(operation_name);
  }

  fmt::print("Span \"{}\" created with parent!\n", operation_name);
  return opentracing::Tracer::Global()->StartSpan(
    operation_name, {opentracing::ChildOf(ctx->get())});
}
} // namespace shared
