#pragma once

#include <any>
#include <cstdint>
#include <memory>  // for std::shared_ptr
#include <string>

#include "symbol/type.h"
#include "instruction/instruction.h"
#include "tokenizer/token.h"
#include "generator/generator.h"
#include "prog_node.h"

namespace miniplc0 {

// 原来这个就是AST的节点
// 表达式的基本单元为Item
// 以下结构用于记录item信息


// enum ItemType { VOID_ITEM, INT_ITEM, DOUBLE_ITEM, CHAR_ITEM, STRING_ITEM };

struct Item {
  TokenType type;
  Generator p_code_gen;

  void combine(TokenType op, std::shared_ptr<miniplc0::Item>);
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
  std::vector<Instruction> body;
};

}  // namespace miniplc0
