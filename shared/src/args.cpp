#include <cstring>

#include <new>
#include <utility>

#include "args.hpp"

namespace shared {
Args Args::create(int argc, char** argv, const std::function<bool(int)>& keep) {
  std::vector<char*> args;

  for (auto i = 0; i < argc; ++i) {
    if (keep(i)) {
      const auto len = strlen(argv[i]);
      auto* p = new (std::nothrow) char[len + 1];

      if (p == nullptr) {
        for (auto* ptr : args)
          delete[] ptr;

        throw std::bad_alloc();
      }

      memcpy(p, argv[i], len + 1);
      args.push_back(p);
    }
  }

  return Args(std::move(args));
}

Args::~Args() {
  for (auto* p : data_)
    delete[] p;
}

int Args::argc() const noexcept {
  return static_cast<int>(data_.size());
}

char** Args::argv() noexcept {
  return data_.data();
}

Args::Args(std::vector<char*>&& data) noexcept : data_(std::move(data)) {
}
} // namespace shared
