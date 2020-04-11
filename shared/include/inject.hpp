#pragma once
#include <caf/string_view.hpp>

#include <opentracing/tracer.h>

#include "aprint.hpp"
#include "error.hpp"

namespace shared {
namespace detail {
tl::expected<std::string, error> inject(const opentracing::SpanContext& sc);
} // namespace detail

template <class Self>
std::string inject(Self&& self, const opentracing::Span& span,
                   caf::string_view operation_name) {
  const auto inject_res = detail::inject(span.context());

  if (!inject_res.has_value())
    aprint(std::forward<Self>(self),
           "Couldn't inject span context for {} \"{}\"\n", operation_name,
           inject_res.error());

  return inject_res.value_or("");
}
} // namespace shared
