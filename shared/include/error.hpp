#pragma once
#include <iosfwd>
#include <string>

#include <tl/expected.hpp>

#include "source_location.hpp"

namespace shared
{
class error
{
public:
  explicit error(std::string error_message);

  [[nodiscard]] const std::string& message() const;

  template <class Exception>
  [[noreturn]] void throw_as() const
  {
    throw Exception(message());
  }

  friend std::ostream& operator<<(std::ostream& os, const error& e);

private:
  std::string error_message_;
};

#define SHARED_MAKE_ERROR(message) \
  ::shared::error(::std::string(message) + "\nerror occurred at:\n" + SHARED_SOURCE_LOCATION)

#define SHARED_UNEXPECTED(message) ::tl::make_unexpected(SHARED_MAKE_ERROR(message))
} // namespace shared

