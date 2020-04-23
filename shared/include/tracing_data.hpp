#pragma once
#include <caf/all.hpp>

#include "span_context.hpp"

namespace shared {
class tracing_data : public caf::tracing_data {
public:
  span_context value;

  explicit tracing_data(span_context span_ctx);

  caf::error serialize(caf::serializer& sink) const override;

  caf::error_code<caf::sec>
  serialize(caf::binary_serializer& sink) const override;
};
} // namespace shared
