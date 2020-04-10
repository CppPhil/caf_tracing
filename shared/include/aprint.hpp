#pragma once
#include <utility>

#include <fmt/format.h>
#include <fmt/ostream.h>

#include "caf/all.hpp"

namespace shared {
/// Formats a message using fmt::format which is then printed to aout.
/// @tparam Self The type of `self`.
/// @tparam FormatString The type of the `format_string`.
/// @tparam Ts A template type parameter pack of the arguments.
/// @param self The actor that wants to print to stdout.
/// @param format_string The fmtlib compatible format string to use.
/// @param xs The arguments to pass to fmt::format.
/// @return `caf::actor_ostream`.
/// @see https://github.com/fmtlib/fmt
/// @see https://fmt.dev/latest/index.html
template <class Self, class FormatString, class... Ts>
caf::actor_ostream
aprint(Self&& self, const FormatString& format_string, Ts&&... xs) {
  return caf::aout(std::forward<Self>(self))
         << fmt::format(format_string, std::forward<Ts>(xs)...) << std::flush;
}
} // namespace shared
