#pragma once

#include <any>
#include <cstdint>
#include <memory>  // for std::shared_ptr
#include <string>

#include "stat_node.h"
#include "symbol/type.h"
#include "tokenizer/token.h"

namespace miniplc0 {

using std::shared_ptr;
using std::vector;

class FuncParamNode : public Node {
 public:
  shared_ptr<ExprNode> _ident;
  TokenType _type;
  bool _const;
  FuncParamNode() { _ntype = NodeType::FuncParamNode; }
};

class FuncNode : public Node {
 public:
  shared_ptr<ExprNode> _ident;
  vector<shared_ptr<FuncParamNode>> _params;
  TokenType _return_type;
  shared_ptr<StatNode> _body;
  FuncNode() { _ntype = NodeType::FuncNode; }
};

typedef std::shared_ptr<FuncNode> FuncNodePtr;
typedef std::shared_ptr<FuncParamNode> FuncParamNodePtr;

}  // namespace miniplc0
