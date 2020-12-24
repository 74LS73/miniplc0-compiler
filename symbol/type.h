#pragma once
#include <vector>

#include "tokenizer/token.h"
#include "instruction/instruction.h"

namespace miniplc0 {

enum class VariableType {
  LOCAL,
  GLOBAL,
  PARAM
};

struct VariableItem {
  int64_t id;
  std::string name;
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
  int64_t id;
  std::string name;
  int64_t param_slots;
  std::vector<VariableItem> params;
  int64_t return_slots;
  TokenType return_type;
  int64_t local_slots;
  bool need_return;
};


}  // namespace miniplc0