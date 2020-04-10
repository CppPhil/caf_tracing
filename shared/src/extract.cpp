#include <sstream>

#include "extract.hpp"

namespace shared {
namespace {
opentracing::expected<std::unique_ptr<opentracing::SpanContext>>
extract_impl(const std::string& string) {
  std::istringstream iss(string);
  return opentracing::Tracer::Global()->Extract(iss);
}
} // namespace

tl::expected<std::unique_ptr<opentracing::SpanContext>, error>
extract(const std::string& string) {
  auto exp = extract_impl(string);

  if (!exp.has_value()) {
    std::ostringstream oss;
    oss << exp.error();
    return SHARED_UNEXPECTED(oss.str());
  }

  return *std::move(exp);
}
} // namespace shared
