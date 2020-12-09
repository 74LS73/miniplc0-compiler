#pragma once

#include <any>
#include <cstdint>
#include <memory>  // for std::shared_ptr
#include <string>

#include "instruction/instruction.h"
#include "tokenizer/token.h"

namespace miniplc0 {

// 原来这个就是AST的节点
// 表达式的基本单元为Item
// 以下结构用于记录item信息
class Item {};

enum ItemType { VOID_ITEM, INT_ITEM, DOUBLE_ITEM, CHAR_ITEM, STRING_ITEM };



enum VariableType {
  LOCAL,
  GLOBAL,
  PARAM
};

struct VariableItem {
  int32_t id;
  TokenType type;
  bool is_const;
  VariableType vt;
};


/**
 * 函数需要记录的信息：
 * 名字
 * 参数的个数
 * 内部局部变量的个数
 * 返回值的个数（0/1）
 */

struct FunctionItem {
  int32_t id;
  std::string name;
  int32_t param_slots;
  std::vector<VariableItem> params;
  int32_t return_slots;
  TokenType return_type;
  int32_t local_slots;
  bool need_return;
  std::vector<Instruction> body;
};

struct OperatorItem : public Item {
  // TODO
  Item _lhs;
  Item _rhs;
  TokenType _operator;
  std::vector<Instruction> body;
  friend void swap(OperatorItem &lhs, OperatorItem &rhs);
  OperatorItem &operator+(const OperatorItem &other) {
    for (auto &ins : other.body) {
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
