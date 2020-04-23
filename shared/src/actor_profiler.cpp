#include <cstdio>

#include <utility>

#include <fmt/format.h>

#include <pl/current_function.hpp>

#include "actor_profiler.hpp"
#include "create_span.hpp"
#include "span_context.hpp"
#include "tracer/get.hpp"
#include "tracing_data.hpp"

namespace shared {
namespace {
thread_local span_context current_span_context;
} // namespace

void set_span_context(const span_context& span_ctx) {
  current_span_context = span_ctx;
}

void set_span_context(span_context&& span_ctx) {
  current_span_context = std::move(span_ctx);
}

actor_profiler::actor_profiler() = default;

void actor_profiler::add_actor(
  [[maybe_unused]] const caf::local_actor& self,
  [[maybe_unused]] const caf::local_actor* parent) {
  // nop
}

void actor_profiler::remove_actor([
  [maybe_unused]] const caf::local_actor& actor) {
  // nop
}

void actor_profiler::before_processing(const caf::local_actor& actor,
                                       const caf::mailbox_element& element) {
  auto* tracer = tracer::get(actor.id());

  if (tracer == nullptr)
    return;

  auto span = create_span(tracer, element.tracing_id.get(),
                          "before_processing");
  /*span->SetTag("actor", actor.name());
  span->SetTag("element", caf::to_string(element.content()));*/
}

void actor_profiler::after_processing(
  [[maybe_unused]] const caf::local_actor& actor,
  [[maybe_unused]] caf::invoke_message_result result) {
  // nop
}

void actor_profiler::before_sending(const caf::local_actor& actor,
                                    caf::mailbox_element& element) {
  auto* tracer = tracer::get(actor.id());

  if (tracer == nullptr)
    return;

  if (element.tracing_id == nullptr)
    element.tracing_id = std::make_unique<tracing_data>(current_span_context);

  auto span = create_span(tracer, element.tracing_id.get(), "before_sending");
  /*
    span->SetTag("actor", actor.name());
    span->SetTag("element", caf::to_string(element.content()));
  */
  const auto inject_res = span_context::inject(tracer, span);

  if (!inject_res.has_value()) {
    fmt::print(stderr, "{}: inject failed!\n", PL_CURRENT_FUNCTION);
    return;
  }

  element.tracing_id = std::make_unique<tracing_data>(*inject_res);
}

void actor_profiler::before_sending_scheduled(
  const caf::local_actor& actor, caf::actor_clock::time_point timeout,
  caf::mailbox_element& element) {
  auto* tracer = tracer::get(actor.id());

  if (tracer == nullptr)
    return;

  if (element.tracing_id == nullptr)
    element.tracing_id = std::make_unique<tracing_data>(current_span_context);

  auto span = create_span(tracer, element.tracing_id.get(),
                          "before_sending_scheduled");
  /*
    span->SetTag("actor", actor.name());
    span->SetTag("timeout", timeout.time_since_epoch().count());
    span->SetTag("element", caf::to_string(element.content()));
  */
  const auto inject_res = span_context::inject(tracer, span);

  if (!inject_res.has_value()) {
    fmt::print(stderr, "{}: inject failed!\n", PL_CURRENT_FUNCTION);
    return;
  }

  element.tracing_id = std::make_unique<tracing_data>(*inject_res);
}
} // namespace shared
