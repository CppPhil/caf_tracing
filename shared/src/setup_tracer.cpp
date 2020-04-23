#include <cstdio>

#include <memory>

#include <yaml-cpp/yaml.h>

#include <jaegertracing/Tracer.h>

#include "setup_tracer.hpp"

namespace shared {
namespace {
class Logger : public jaegertracing::logging::Logger {
public:
  Logger() {
    setbuf(stdout, nullptr);
  }

  ~Logger() override = default;

  void error(const std::string& message) override {
    fprintf(stderr, "JAEGER ERROR: \"%s\"\n", message.c_str());
  }

  void info(const std::string& message) override {
    printf("JAEGER INFO : \"%s\"\n", message.c_str());
  }
};
} // namespace

void setup_tracer(const std::string& config_filepath,
                  const std::string& service) {
  try {
    auto config_yaml = YAML::LoadFile(config_filepath);
    auto config = jaegertracing::Config::parse(config_yaml);
    auto tracer = jaegertracing::Tracer::make(service, config,
                                              std::make_unique<Logger>());

    opentracing::Tracer::InitGlobal(
      std::static_pointer_cast<opentracing::Tracer>(tracer));

    printf("JAEGER INFO: tracer setup for \"%s\"\n", service.c_str());
  } catch (const std::runtime_error& ex) {
    if (strstr(ex.what(), "Cannot connect socket to remote address") != nullptr)
      fprintf(stderr, "Exception caught: \"%s\"\nIs Jaeger running?\n",
              ex.what());

    throw;
  }
}
} // namespace shared
