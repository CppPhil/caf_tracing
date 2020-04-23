#include "tracing_data_factory.hpp"

namespace shared {
tracing_data_factory::tracing_data_factory() : caf::tracing_data_factory() {
  // nop
}

tracing_data_factory::~tracing_data_factory() = default;

caf::error tracing_data_factory::deserialize(
  caf::deserializer& source, std::unique_ptr<caf::tracing_data>& dst) const {
  return deserialize_impl(source, dst);
}

caf::error_code<caf::sec> tracing_data_factory::deserialize(
  caf::binary_deserializer& source,
  std::unique_ptr<caf::tracing_data>& dst) const {
  return deserialize_impl(source, dst);
}
} // namespace shared
