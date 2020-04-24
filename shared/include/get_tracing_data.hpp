#pragma once
#include <cstdio>

#include <fmt/format.h>

#include <caf/all.hpp>

namespace shared {
template <class T>
caf::tracing_data* get_tracing_data(T& actor) {
  auto* mailbox_element = actor->current_mailbox_element();

  if (mailbox_element == nullptr) {
    fmt::print(stderr,
               "get_tracing_data: current mailbox_element is NULLPTR!\n");
    return nullptr;
  }

  auto* p = mailbox_element->tracing_id.get();

  if (p == nullptr) {
    fmt::print(stderr, "get_tracing_data: tracing_id is NULLPTR!\n");
  }

  return p;
}
} // namespace shared
