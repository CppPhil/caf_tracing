#pragma once
#include "span_context.hpp"

#include <gsl/gsl_util>

#include <caf/all.hpp>

#include <tuple>
#include <utility>

namespace shared {
template <class... Ts>
class message {
public:
  message() = default;

  explicit message(span_context span_ctx, Ts... xs)
    : span_context_(std::move(span_ctx)), tuple_(std::move(xs)...) {
  }

  span_context& span_ctx() noexcept {
    return span_context_;
  }

  const span_context& span_ctx() const noexcept {
    return const_cast<message*>(this)->span_ctx();
  }

  std::tuple<Ts...>& tuple() noexcept {
    return tuple_;
  }

  const std::tuple<Ts...>& tuple() const noexcept {
    return const_cast<message*>(this)->tuple();
  }

  template <class Inspector>
  friend std::enable_if_t<Inspector::reads_state,
                          typename Inspector::result_type>
  inspect(Inspector& f, const message& msg) {
    return f(caf::meta::type_name("message"), msg.span_context_, msg.tuple_);
  }

  template <class Inspector>
  friend std::enable_if_t<Inspector::writes_state,
                          typename Inspector::result_type>
  inspect(Inspector& f, message& msg) {
    span_context span_ctx;
    std::tuple<Ts...> tuple;
    auto g = gsl::finally([&] {
      msg.span_context_ = span_ctx;
      msg.tuple_ = tuple;
    });
    return f(caf::meta::type_name("message"), span_ctx, tuple);
  }

private:
  span_context span_context_;
  std::tuple<Ts...> tuple_;
};
} // namespace shared
