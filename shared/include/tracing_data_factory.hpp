#pragma once
#include <caf/all.hpp>
#include <caf/tracing_data_factory.hpp>

#include <memory>

#include "tracing_data.hpp"

namespace shared {
class tracing_data_factory : public caf::tracing_data_factory {
public:
  tracing_data_factory();

  ~tracing_data_factory() override;

  caf::error
  deserialize(caf::deserializer& source,
              std::unique_ptr<caf::tracing_data>& dst) const override;

  caf::error_code<caf::sec>
  deserialize(caf::binary_serializer& source,
              std::unique_ptr<caf::tracing_data>& dst) const override;

private:
  template <class Deserializer>
  typename Deserializer::result_type
  deserialize_impl(Deserializer& source,
                   std::unique_ptr<caf::tracing_data>& dst) const {
    span_context value;
    if (auto err = source(value))
      return err;
    dst = std::make_unique<tracing_data>(std::move(value));
    return {};
  }
};
} // namespace shared
