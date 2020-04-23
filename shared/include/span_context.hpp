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

  const std::string& buf() const noexcept;

  friend std::ostream& operator<<(std::ostream& os,
                                  const span_context& span_ctx);

  template <class Inspector>
  friend std::enable_if_t<Inspector::reads_state,
                          typename Inspector::result_type>
  inspect(Inspector& f, const span_context& span_ctx) {
    return f(caf::meta::type_name("span_context"), span_ctx.buffer_);
  }

  template <class Inspector>
  friend std::enable_if_t<Inspector::writes_state,
                          typename Inspector::result_type>
  inspect(Inspector& f, span_context& span_ctx) {
    std::string string;
    auto g = gsl::finally([&] { span_ctx.buffer_ = string; });
    return f(caf::meta::type_name("span_context"), string);
  }

private:
  std::string buffer_;
};
} // namespace shared
