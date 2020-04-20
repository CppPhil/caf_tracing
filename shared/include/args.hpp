#pragma once
#include <functional>
#include <memory>
#include <vector>

namespace shared {
class args {
public:
  args(
    int argc, char** argv,
    const std::function<bool(int)>& keep = [](int) { return true; });

  int argc() const noexcept;

  char** argv() noexcept;

private:
  std::vector<std::unique_ptr<char[]>> data_;
  std::vector<char*> argv_;
};
} // namespace shared
