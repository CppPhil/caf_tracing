#pragma once
#include <functional>
#include <vector>

namespace shared {
class args {
public:
  args(
    int argc, char** argv,
    const std::function<bool(int)>& keep = [](int) { return true; });

  ~args();

  int argc() const noexcept;

  char** argv() noexcept;

private:
  std::vector<char*> data_;
};
} // namespace shared
