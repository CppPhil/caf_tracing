#include <sstream>

#include "inject.hpp"

namespace shared{
tl::expected<std::string, error> inject(const opentracing::SpanContext& sc)
{
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
} // namespace shared
