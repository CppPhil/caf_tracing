#include <cstring>

#include <new>
#include <utility>

#include "args.hpp"

namespace shared {
args::args(int argc, char** argv, const std::function<bool(int)>& keep) {
  for (auto i = 0; i < argc; ++i) {
    if (keep(i)) {
      const auto len = strlen(argv[i]);
      auto* p = new (std::nothrow) char[len + 1];

      if (p == nullptr) {
        for (auto* ptr : data_)
          delete[] ptr;

        throw std::bad_alloc();
      }

      memcpy(p, argv[i], len + 1);
      data_.push_back(p);
    }
  }
}

args::~args() {
  for (auto* p : data_)
    delete[] p;
}

int args::argc() const noexcept {
  return static_cast<int>(data_.size());
}

char** args::argv() noexcept {
  return data_.data();
}
} // namespace shared
