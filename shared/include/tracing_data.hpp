#pragma once
#include <caf/all.hpp>

namespace shared {
class tracing_data : public caf::tracing_data {
public:
  std::string value;

  tracing_data(std::string span_ctx);

  caf::error serialize(caf::serializer& sink) const override;

  caf::error_code<caf::sec>
  serialize(caf::binary_serializer& sink) const override;
};
} // namespace shared
