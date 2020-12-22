#pragma once

#include <any>
#include <cstdint>
#include <memory>  // for std::shared_ptr
#include <string>

#include "generator/generator.h"
#include "instruction/instruction.h"
#include "node.h"
#include "symbol/type.h"
#include "tokenizer/token.h"

namespace miniplc0 {

using std::shared_ptr;

class ExprNode : public Node {
 public:
  TokenType _type;
  ExprNode() : _type(TokenType::VOID) { _ntype = NodeType::ExprNode; }
};

struct AssignExprNode : public ExprNode {
  shared_ptr<ExprNode> _lhs;
  shared_ptr<ExprNode> _rhs;
  AssignExprNode() { _ntype = NodeType::AssignExprNode; }
};

class ItemExprNode : public ExprNode {
 public:
  int64_t *_value;
  ItemExprNode() { _ntype = NodeType::ItemExprNode; }
};

class IdentExprNode : public ExprNode {
 public:
  std::string _name;
  IdentExprNode() { _ntype = NodeType::IdentExprNode; }
};

class CallExprNode : public ExprNode {
 public:
  std::string _name;
  std::vector<shared_ptr<ExprNode>> _params;
  CallExprNode() { _ntype = NodeType::CallExprNode; }
};

class UnaryExprNode : public ExprNode {
 public:
  bool _nega;
  shared_ptr<ExprNode> _ident;
  UnaryExprNode() : _nega(false) { _ntype = NodeType::UnaryExprNode; }
  UnaryExprNode &operator=(UnaryExprNode &other) {
    // swap(*this, other);
    return *this;
  }
};

class OpExprNode : public UnaryExprNode {
 public:
  shared_ptr<ExprNode> _lhs;
  shared_ptr<ExprNode> _rhs;
  TokenType _operator;
  OpExprNode() { _ntype = NodeType::OpExprNode; }
  friend void swap(OpExprNode &lhs, OpExprNode &rhs);
  OpExprNode &operator=(UnaryExprNode &other) {
    // swap(*this, other);
    return *this;
  }
};

typedef std::shared_ptr<ExprNode> ExprNodePtr;
typedef std::shared_ptr<AssignExprNode> AssignExprNodePtr;
typedef std::shared_ptr<ItemExprNode> ItemExprNodePtr;
typedef std::shared_ptr<IdentExprNode> IdentExprNodePtr;
typedef std::shared_ptr<CallExprNode> CallExprNodePtr;
typedef std::shared_ptr<UnaryExprNode> UnaryExprNodePtr;
typedef std::shared_ptr<OpExprNode> OpExprNodePtr;

inline void swap(OpExprNode &lhs, OpExprNode &rhs) {
  using std::swap;
  // swap(lhs.body, rhs.body);
}

inline void swap(UnaryExprNode &lhs, UnaryExprNode &rhs) {
  using std::swap;
  swap(lhs._nega, rhs._nega);
  swap(lhs._ident, rhs._ident);
}

}  // namespace miniplc0
