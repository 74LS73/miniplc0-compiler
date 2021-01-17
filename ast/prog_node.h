#pragma once

#include <any>
#include <cstdint>
#include <memory>  // for std::shared_ptr
#include <string>

#include "func_node.h"
#include "symbol/type.h"
#include "symbol/symbol_table.h"

namespace miniplc0 {

using std::shared_ptr;
using std::vector;


class ProgNode : public Node {
 public:
  /// 魔数
  uint32_t _magic = 0x72303b3e;
  /// 版本号，定为 1
  uint32_t _version = 0x00000001;
  vector<shared_ptr<DeclStatNode>> _vars;
  vector<shared_ptr<FuncNode>> _funcs;
  SymbolTable *_globals;

  ProgNode() { _ntype = NodeType::ProgNode; }
};

typedef std::shared_ptr<ProgNode> ProgNodePtr;

}  // namespace miniplc0
