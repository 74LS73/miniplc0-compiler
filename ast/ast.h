#pragma once

#include <any>
#include <cstdint>
#include <memory>  // for std::shared_ptr
#include <string>

#include "symbol/type.h"
#include "tokenizer/token.h"
#include "prog_node.h"

namespace miniplc0 {

// 原来这个就是AST的节点
// 表达式的基本单元为Item
// 以下结构用于记录item信息


// enum ItemType { VOID_ITEM, INT_ITEM, DOUBLE_ITEM, CHAR_ITEM, STRING_ITEM };

struct Item {
  TokenType type;

  void combine(TokenType op, std::shared_ptr<miniplc0::Item>);
};






}  // namespace miniplc0
