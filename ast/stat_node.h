#pragma once

#include <any>
#include <cstdint>
#include <memory>  // for std::shared_ptr
#include <string>

#include "expr_node.h"
#include "symbol/type.h"
#include "tokenizer/token.h"

namespace miniplc0 {

using std::shared_ptr;

class StatNode : public Node {
 public:
  // std::vector<shared_ptr<ExprNode>> _exprs;
};

class ExprStatNode : public StatNode {
 public:
  shared_ptr<ExprNode> _expr;
};

class BlockStatNode : public StatNode {
 public:
  std::vector<shared_ptr<StatNode>> _stats;
};

class DeclStatNode : public StatNode {
 public:
  bool _const;
  shared_ptr<ExprNode> _var;
  shared_ptr<ExprNode> _value;
};

class IfStatNode : public StatNode {
 public:
  shared_ptr<ExprNode> _expr;
  shared_ptr<StatNode> _if_block;
  shared_ptr<StatNode> _else_block;
};

class WhileStatNode : public StatNode {
 public:
  shared_ptr<ExprNode> _expr;
  shared_ptr<StatNode> _block;
};

class BreakStatNode : public StatNode {};
class ContinueStatNode : public StatNode {};
class ReturnStatNode : public StatNode {
 public:
  shared_ptr<ExprNode> _expr;
};

typedef std::shared_ptr<StatNode> StatNodePtr;
typedef std::shared_ptr<ExprStatNode> ExprStatNodePtr;
typedef std::shared_ptr<BlockStatNode> BlockStatNodePtr;
typedef std::shared_ptr<DeclStatNode> DeclStatNodePtr;
typedef std::shared_ptr<IfStatNode> IfStatNodePtr;
typedef std::shared_ptr<WhileStatNode> WhileStatNodePtr;
typedef std::shared_ptr<BreakStatNode> BreakStatNodePtr;
typedef std::shared_ptr<ContinueStatNode> ContinueStatNodePtr;
typedef std::shared_ptr<ReturnStatNode> ReturnStatNodePtr;

}  // namespace miniplc0
