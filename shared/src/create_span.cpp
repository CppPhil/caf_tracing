#include <cstdio>

#include <fmt/format.h>

#include "create_span.hpp"

namespace shared {
std::unique_ptr<opentracing::Span>
create_span(const opentracing::Tracer* tracer,
            const span_context& serialized_span_context,
            const std::string& operation_name) {
  const auto ctx = serialized_span_context.extract(tracer);

  if (!ctx.has_value() || *ctx == nullptr) {
    fmt::print(stderr, "Span \"{}\" has no parent!\n", operation_name);
    return tracer->StartSpan(operation_name);
  }

  fmt::print("Span \"{}\" created with parent!\n", operation_name);
  return tracer->StartSpan(operation_name, {opentracing::ChildOf(ctx->get())});
}
} // namespace shared
