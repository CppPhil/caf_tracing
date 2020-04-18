#pragma once
#include <caf/all.hpp>

#include "span_context.hpp"

namespace shared::traced {
template <class... Ts>
struct replies_to {
  template <class... Ts2>
  using with =
    typename caf::replies_to<Ts..., span_context>::template with<Ts2...,
                                                                 span_context>;
};
} // namespace shared::traced
