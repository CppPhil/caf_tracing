#pragma once
#include <caf/all.hpp>

#include "message.hpp"

namespace shared::traced {
template <class Atom, class... Ts>
struct replies_to {
  template <class... Ts2>
  using with =
    typename caf::replies_to<Atom,
                             message<Ts...>>::template with<message<Ts2...>>;
};
} // namespace shared::traced
