#pragma once
#include <utility>

#include "span_context.hpp"

namespace shared {
template <class T>
class tracing_sender {
public:
  tracing_sender(T self, opentracing::string_view operation_name)
    : self_(std::move(self)),
      span_(opentracing::Tracer::Global()->StartSpan(operation_name)) {
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
    span_->BaggageItem(restricted_key);
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

  template <class... Ts>
  decltype(auto) send(Ts&&... xs) {
    return self_->send(
      std::forward<Ts>(xs)...,
      span_context::inject(span_).value_or((shared::span_context())));
  }

private:
  T self_;
  std::unique_ptr<opentracing::Span> span_;
};
} // namespace shared
