#include "tracing_data.hpp"

namespace shared {
tracing_data::tracing_data(const span_context& span_ctx)
  : value(span_ctx.buf()) {
  // nop
}

caf::error tracing_data::serialize(caf::serializer& sink) const {
  return sink(value);
}

caf::error_code<caf::sec>
tracing_data::serialize(caf::binary_serializer& sink) const {
  return sink(value);
}
} // namespace shared
