#include <utility>

#include "inject.hpp"
#include "span_context.hpp"

namespace shared {
namespace {
tl::expected<std::string, error>
inject_impl(const opentracing::SpanContext& sc) {
  std::ostringstream oss;

  const auto exp = opentracing::Tracer::Global()->Inject(sc, oss);

  if (!exp.has_value()) {
    oss.clear();
    oss.str("");

    oss << exp.error();

    return SHARED_UNEXPECTED(oss.str());
  }

  return oss.str();
}

opentracing::expected<std::unique_ptr<opentracing::SpanContext>>
extract_impl(const std::string& string) {
  std::istringstream iss(string);
  return opentracing::Tracer::Global()->Extract(iss);
}
} // namespace

tl::expected<span_context, error>
span_context::inject(const opentracing::SpanContext& span_ctx) {
  auto expected_serialized_span_context = inject_impl(span_ctx);

  if (!expected_serialized_span_context.has_value())
    return tl::make_unexpected(expected_serialized_span_context.error());

  return span_context(*std::move(expected_serialized_span_context));
}

tl::expected<span_context, error>
span_context::inject(const opentracing::Span& span) {
  return create(span.context());
}

tl::expected<std::unique_ptr<opentracing::SpanContext>, error> extract() const {
  auto exp = extract_impl(string);

  if (!exp.has_value()) {
    std::ostringstream oss;
    oss << exp.error();
    return SHARED_UNEXPECTED(oss.str());
  }

  return *std::move(exp);
}

span_context::span_context(std::string&& serialized_span_context)
  : buffer_(std::move(serialized_span_context)) {
}
} // namespace shared
