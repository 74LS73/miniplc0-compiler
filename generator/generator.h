#pragma once

#include <instruction/instruction.h>

#include <utility>
#include <vector>

namespace miniplc0 {

class Generator final {
 public:
  Generator() = default;

 public:
  void add();

 private:
  std::vector<Instruction> _instructions;
};

}  // namespace miniplc0
