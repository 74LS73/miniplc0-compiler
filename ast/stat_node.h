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
  StatNode() { _ntype = NodeType::StatNode; }
};

class ExprStatNode : public StatNode {
 public:
  shared_ptr<ExprNode> _expr;
  ExprStatNode() { _ntype = NodeType::ExprStatNode; }
};

class BlockStatNode : public StatNode {
 public:
  std::vector<shared_ptr<StatNode>> _stats;
  BlockStatNode() { _ntype = NodeType::BlockStatNode; }
};

class DeclStatNode : public StatNode {
 public:
  bool _const;
  std::string _name;
  shared_ptr<ExprNode> _value;
  TokenType _type;
  DeclStatNode() { _ntype = NodeType::DeclStatNode; }
};

class IfStatNode : public StatNode {
 public:
  shared_ptr<ExprNode> _expr;
  shared_ptr<StatNode> _if_block;
  shared_ptr<StatNode> _else_block;
  IfStatNode() { _ntype = NodeType::IfStatNode; }
};

class WhileStatNode : public StatNode {
 public:
  shared_ptr<ExprNode> _expr;
  shared_ptr<StatNode> _block;
  WhileStatNode() { _ntype = NodeType::WhileStatNode; }
};

class BreakStatNode : public StatNode {
 public:
  BreakStatNode() { _ntype = NodeType::BreakStatNode; }
};
class ContinueStatNode : public StatNode {
 public:
  ContinueStatNode() { _ntype = NodeType::ContinueStatNode; }
};
class ReturnStatNode : public StatNode {
 public:
  shared_ptr<ExprNode> _expr;
  ReturnStatNode() { _ntype = NodeType::ReturnStatNode; }
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
