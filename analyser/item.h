#pragma once

#include <any>
#include <cstdint>
#include <memory>  // for std::shared_ptr
#include <string>

#include "analyser.h"
#include "error/error.h"

namespace miniplc0 {

// 表达式的基本单元为Item
// 以下结构用于记录item信息
struct OperatorItem {
  // TODO
  std::vector<Instruction> body;
  friend void swap(OperatorItem &lhs, OperatorItem &rhs);
  OperatorItem & operator+(const OperatorItem &other) {
    for (auto  &ins : other.body) {
      this->body.push_back(ins);
    }
    return *this;
  }
  OperatorItem &operator=(OperatorItem &other) {
    swap(*this, other);
    return *this;
  }
};

inline void swap(OperatorItem &lhs, OperatorItem &rhs) {
  using std::swap;
  swap(lhs.body, rhs.body);
}

}  // namespace miniplc0