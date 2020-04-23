#include <ostream>
#include <utility>

#include "error.hpp"

namespace shared {
error::error(std::string error_message)
  : error_message_(std::move(error_message)) {
  // nop
}

const std::string& error::message() const {
  return error_message_;
}

std::ostream& operator<<(std::ostream& os, const error& e) {
  return os << "error{\"message\": \"" << e.message() << "\"}";
}
} // namespace shared
