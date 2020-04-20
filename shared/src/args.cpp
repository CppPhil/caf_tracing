#include <cstring>

#include <new>
#include <utility>

#include "args.hpp"

namespace shared {
args::args(int argc, char** argv, const std::function<bool(int)>& keep) {
  for (auto i = 0; i < argc; ++i) {
    if (keep(i)) {
      const auto len = strlen(argv[i]);
      auto p = std::make_unique<char[]>(len + 1);
      memcpy(p.get(), argv[i], len + 1);
      data_.push_back(std::move(p));
    }
  }

  for (auto& up : data_)
    argv_.push_back(up.get());
}

int args::argc() const noexcept {
  return static_cast<int>(argv_.size());
}

char** args::argv() noexcept {
  return argv_.data();
}
} // namespace shared
