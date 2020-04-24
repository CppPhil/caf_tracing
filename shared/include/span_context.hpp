#pragma once
#include <iosfwd>
#include <string>
#include <type_traits>

#include <gsl/gsl_util>

#include <caf/all.hpp>

#include <opentracing/tracer.h>

#include "error.hpp"

namespace shared {
class span_context {
public:
  static tl::expected<span_context, error>
  inject(const opentracing::Tracer* tracer,
         const opentracing::SpanContext& span_ctx);

  static tl::expected<span_context, error>
  inject(const opentracing::Tracer* tracer, const opentracing::Span& span);

  static tl::expected<span_context, error>
  inject(const opentracing::Tracer* tracer, const opentracing::Span* span_ptr);

  static tl::expected<span_context, error>
  inject(const opentracing::Tracer* tracer,
         const std::unique_ptr<opentracing::Span>& span_unique_ptr);

  span_context();

  explicit span_context(std::string serialized_span_context);

  tl::expected<std::unique_ptr<opentracing::SpanContext>, error>
  extract(const opentracing::Tracer* tracer) const;

  const std::string& str() const noexcept;

  friend std::ostream& operator<<(std::ostream& os,
                                  const span_context& span_ctx);

private:
  std::string buffer_;
};
} // namespace shared
