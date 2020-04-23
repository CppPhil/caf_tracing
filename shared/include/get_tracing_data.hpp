#pragma once
#include <caf/all.hpp>

namespace shared {
template <class T>
caf::tracing_data* get_tracing_data(T& actor) {
  auto* mailbox_element = actor.current_mailbox_element();

  if (mailbox_element == nullptr)
    return nullptr;

  return mailbox_element->tracing_id.get();
}
} // namespace shared
