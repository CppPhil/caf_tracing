#pragma once
#include <caf/all.hpp>

#include "span_context.hpp"

namespace shared {
void set_span_context(const span_context& span_ctx);

void set_span_context(span_context&& span_ctx);

class actor_profiler : public caf::actor_profiler {
public:
  actor_profiler();

  void add_actor(const caf::local_actor& actor,
                 const caf::local_actor* parent) override;

  void remove_actor(const caf::local_actor& actor) override;

  void before_processing(const caf::local_actor& actor,
                         const caf::mailbox_element& element) override;

  void after_processing(const caf::local_actor& actor,
                        caf::invoke_message_result result) override;

  void before_sending(const caf::local_actor& actor,
                      caf::mailbox_element& element) override;

  void before_sending_scheduled(const caf::local_actor& self,
                                caf::actor_clock::time_point timeout,
                                caf::mailbox_element& element) override;
};
} // namespace shared
