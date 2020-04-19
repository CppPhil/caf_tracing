#pragma once
#include <utility>

#include <caf/all.hpp>

#include <tl/optional.hpp>

#include <pl/meta/remove_cvref.hpp>

#include "create_span.hpp"
#include "message.hpp"

namespace shared {
template <class T>
class tracing_sender {
public:
  tracing_sender(T self, opentracing::string_view operation_name,
                 tl::optional<span_context> parent_span = tl::nullopt)
    : self_(std::move(self)),
      span_(parent_span.has_value()
              ? create_span(*parent_span, std::string(operation_name.begin(),
                                                      operation_name.end()))
              : opentracing::Tracer::Global()->StartSpan(operation_name)) {
  }

  void Finish(std::initializer_list<
                opentracing::option_wrapper<opentracing::FinishSpanOption>>
                option_list
              = {}) noexcept {
    span_->Finish(std::move(option_list));
  }

  void FinishWithOptions(
    const opentracing::FinishSpanOptions& finish_span_options) noexcept {
    span_->FinishWithOptions(finish_span_options);
  }

  void SetOperationName(opentracing::string_view name) noexcept {
    span_->SetOperationName(name);
  }

  void SetTag(opentracing::string_view key,
              const opentracing::Value& value) noexcept {
    span_->SetTag(key, value);
  }

  void SetBaggageItem(opentracing::string_view restricted_key,
                      opentracing::string_view value) noexcept {
    span_->SetBaggageItem(restricted_key, value);
  }

  std::string BaggageItem(opentracing::string_view restricted_key) const {
    return span_->BaggageItem(restricted_key);
  }

  void Log(std::initializer_list<
           std::pair<opentracing::string_view, opentracing::Value>>
             fields) noexcept {
    span_->Log(std::move(fields));
  }

  void Log(opentracing::SystemTime timestamp,
           std::initializer_list<
             std::pair<opentracing::string_view, opentracing::Value>>
             fields) noexcept {
    span_->Log(std::move(timestamp), std::move(fields));
  }

  void Log(
    opentracing::SystemTime timestamp,
    const std::vector<std::pair<opentracing::string_view, opentracing::Value>>&
      fields) noexcept {
    span_->Log(std::move(timestamp), fields);
  }

  const opentracing::SpanContext& context() const noexcept {
    return span_->context();
  }

  const opentracing::Tracer& tracer() const noexcept {
    return span_->tracer();
  }

  T underlying() const {
    return self_;
  }

  // TODO: Consider making these so that they're called using operator->
  template <caf::message_priority P = caf::message_priority::normal, class Atom,
            class... Ts>
  void send(const caf::group& dest, Atom&& atom, Ts&&... xs) {
    self_->send(dest, std::forward<Atom>(atom),
                make_message(std::forward<Ts>(xs)...));
  }

  template <caf::message_priority P = caf::message_priority::normal, class Dest,
            class Atom, class... Ts>
  void send(const Dest& dest, Atom&& atom, Ts&&... xs) {
    self_->send(dest, std::forward<Atom>(atom),
                make_message(std::forward<Ts>(xs)...));
  }

  template <caf::message_priority P = caf::message_priority::normal,
            class Dest = caf::actor, class Atom, class... Ts>
  void anon_send(const Dest& dest, Atom&& atom, Ts&&... xs) {
    self_->anon_send(dest, std::forward<Atom>(atom),
                     make_message(std::forward<Ts>(xs)...));
  }

  template <caf::message_priority P = caf::message_priority::normal,
            class Dest = caf::actor, class Atom, class... Ts>
  void scheduled_send(const Dest& dest, caf::actor_clock::time_point timeout,
                      Atom&& atom, Ts&&... xs) {
    self_->scheduled_send(dest, std::move(timeout), std::forward<Atom>(atom),
                          make_message(std::forward<Ts>(xs)...));
  }

  template <class Atom, class... Ts>
  void scheduled_send(const caf::group& dest,
                      caf::actor_clock::time_point timeout, Atom&& atom,
                      Ts&&... xs) {
    self_->scheduled_send(dest, std::move(timeout), std::forward<Atom>(atom),
                          make_message(std::forward<Ts>(xs)...));
  }

  template <caf::message_priority P = caf::message_priority::normal,
            class Rep = int, class Period = std::ratio<1>,
            class Dest = caf::actor, class Atom, class... Ts>
  void delayed_send(const Dest& dest,
                    std::chrono::duration<Rep, Period> rel_timeout, Atom&& atom,
                    Ts&&... xs) {
    self_->delayed_send(dest, std::move(rel_timeout), std::forward<Atom>(atom),
                        make_message(std::forward<Ts>(xs)...));
  }

  template <class Rep = int, class Period = std::ratio<1>,
            class Dest = caf::actor, class Atom, class... Ts>
  void delayed_send(const caf::group& dest,
                    std::chrono::duration<Rep, Period> rtime, Atom&& atom,
                    Ts&&... xs) {
    self_->delayed_send(dest, std::move(rtime), std::forward<Atom>(atom),
                        make_message(std::forward<Ts>(xs)...));
  }

  template <caf::message_priority P = caf::message_priority::normal,
            class Dest = caf::actor, class Atom, class... Ts>
  void scheduled_anon_send(const Dest& dest,
                           caf::actor_clock::time_point timeout, Atom&& atom,
                           Ts&&... xs) {
    self_->scheduled_anon_send(dest, std::move(timeout),
                               std::forward<Atom>(atom),
                               make_message(std::forward<Ts>(xs)...));
  }

  template <caf::message_priority P = caf::message_priority::normal,
            class Dest = caf::actor, class Rep = int,
            class Period = std::ratio<1>, class Atom, class... Ts>
  void delayed_anon_send(const Dest& dest,
                         std::chrono::duration<Rep, Period> rel_timeout,
                         Atom&& atom, Ts&&... xs) {
    self_->delayed_anon_send(dest, std::move(rel_timeout),
                             std::forward<Atom>(atom),
                             make_message(std::forward<Ts>(xs)...));
  }

  template <class Rep = int, class Period = std::ratio<1>, class Atom,
            class... Ts>
  void delayed_anon_send(const caf::group& dest,
                         std::chrono::duration<Rep, Period> rtime, Atom&& atom,
                         Ts&&... xs) {
    self_->delayed_anon_send(dest, std::move(rtime), std::forward<Atom>(atom),
                             make_message(std::forward<Ts>(xs)...));
  }

  template <caf::message_priority P = caf::message_priority::normal,
            class Handle = caf::actor, class Atom, class... Ts>
  auto request(const Handle& dest, const caf::duration& timeout, Atom&& atom,
               Ts&&... xs) {
    return self_->request(dest, timeout, std::forward<Atom>(atom),
                          make_message(std::forward<Ts>(xs)...));
  }

  template <caf::message_priority P = caf::message_priority::normal,
            class Rep = int, class Period = std::ratio<1>,
            class Handle = caf::actor, class Atom, class... Ts>
  auto request(const Handle& dest, std::chrono::duration<Rep, Period> timeout,
               Atom&& atom, Ts&&... xs) {
    return self_->request(dest, std::move(timeout), std::forward<Atom>(atom),
                          make_message(std::forward<Ts>(xs)...));
  }

private:
  template <class... Ts>
  message<pl::meta::remove_cvref_t<Ts>...> make_message(Ts&&... xs) {
    return message(span_context::inject(span_).value_or((span_context())),
                   std::forward<Ts>(xs)...);
  }

  T self_;
  std::unique_ptr<opentracing::Span> span_;
};
} // namespace shared
