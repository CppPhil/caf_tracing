#include <cstdio>

#include <fmt/format.h>

#include "create_span.hpp"
#include "tracing_data.hpp"

namespace shared {
std::unique_ptr<opentracing::Span>
create_span(const opentracing::Tracer* tracer, const caf::tracing_data* data,
            const std::string& operation_name) {
  const auto* p = dynamic_cast<const tracing_data*>(data);

  if (p == nullptr) {
    fmt::print(stderr, "COULDNT DOWNCAST\n");
    fmt::print(stderr, "Span \"{}\" has no parent!\n", operation_name);
    return tracer->StartSpan(operation_name);
  }

  const auto ctx = span_context(p->value).extract(tracer);

  if (!ctx.has_value()) {
    fmt::print(stderr, "EXTRACT FAILURE\n");
    fmt::print(stderr, "Span \"{}\" has no parent!\n", operation_name);
    return tracer->StartSpan(operation_name);
  }

  if (*ctx == nullptr) {
    fmt::print("*ctx WAS NULLPTR\n");
    fmt::print(stderr, "Span \"{}\" has no parent!\n", operation_name);
    return tracer->StartSpan(operation_name);
  }

  fmt::print("Span \"{}\" created with parent!\n", operation_name);
  return tracer->StartSpan(operation_name, {opentracing::ChildOf(ctx->get())});
}
} // namespace shared
