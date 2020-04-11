#pragma once
#include <functional>
#include <vector>

namespace shared {
class Args {
public:
  static Args create(
    int argc, char** argv,
    const std::function<bool(int)>& keep = [](int) { return true; });

  ~Args();

  int argc() const noexcept;

  char** argv() noexcept;

private:
  explicit Args(std::vector<char*>&& data) noexcept;

  std::vector<char*> data_;
};
} // namespace shared
