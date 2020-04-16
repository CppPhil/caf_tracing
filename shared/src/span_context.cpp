#include <cstdio>

#include <ostream>
#include <utility>

#include <fmt/format.h>

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

  if (!expected_serialized_span_context.has_value()) {
    fmt::print(stderr, "INJECT FAILURE: \"{}\"\n",
               expected_serialized_span_context.error());
    return tl::make_unexpected(expected_serialized_span_context.error());
  }

  return span_context(*std::move(expected_serialized_span_context));
}

tl::expected<span_context, error>
span_context::inject(const opentracing::Span& span) {
  return inject(span.context());
}

tl::expected<span_context, error>
span_context::inject(const opentracing::Span* span_ptr) {
  if (span_ptr == nullptr) {
    const tl::expected<span_context, error> error_result(
      SHARED_UNEXPECTED("span_ptr was equal to nullptr!"));
    fmt::print(stderr, "INJECT FAILURE: \"{}\"\n", error_result.error());
    return error_result;
  }

  return inject(*span_ptr);
}

tl::expected<span_context, error> span_context::inject(
  const std::unique_ptr<opentracing::Span>& span_unique_ptr) {
  return inject(span_unique_ptr.get());
}

tl::expected<std::unique_ptr<opentracing::SpanContext>, error>
span_context::extract() const {
  auto exp = extract_impl(buffer_);

  if (!exp.has_value()) {
    std::ostringstream oss;
    oss << exp.error();
    auto str = oss.str();
    fmt::print(stderr, "EXTRACT FAILURE: \"{}\"\n", str);
    return SHARED_UNEXPECTED(std::move(str));
  }

  return *std::move(exp);
}

std::ostream& operator<<(std::ostream& os, const span_context& span_ctx) {
  return os << fmt::format("span_context{\"buffer_\": \"{}\"}",
                           span_ctx.buffer_);
}

span_context::span_context(std::string&& serialized_span_context)
  : buffer_(std::move(serialized_span_context)) {
}
} // namespace shared
