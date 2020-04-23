#include "profiler.hpp"

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

profiler::profiler() = default;

void profiler::add_actor([[maybe_unused]] const caf::local_actor& self,
                         [[maybe_unused]] const caf::local_actor* parent) {
  // nop
}

void profiler::remove_actor([[maybe_unused]] const caf::local_actor& actor) {
  // nop
}

void profiler::before_processing(const caf::local_actor& actor,
                                 const caf::mailbox_element& element) {
#warning "TODO: HERE"
  // TODO: HERE
}

void profiler::after_processing(
  [[maybe_unused]] const caf::local_actor& actor,
  [[maybe_unused]] caf::invoke_message_result result) {
  // nop
}

void profiler::before_sending(const caf::local_actor& actor,
                              caf::mailbox_element& element) {
#warning "TODO: HERE"
  // TODO: HERE
}

void profiler::before_sending_scheduled(const caf::local_actor& actor,
                                        caf::actor_clock::time_point timeout,
                                        caf::mailbox_element& element) {
#warning "TODO: HERE"
  // TODO: HERE
}
} // namespace shared
