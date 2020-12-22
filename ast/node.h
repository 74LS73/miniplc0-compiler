#pragma once

#include <any>
#include <cstdint>
#include <memory>  // for std::shared_ptr
#include <string>

namespace miniplc0 {

enum class NodeType {
  Node,
  ExprNode,
  AssignExprNode,
  ItemExprNode,
  IdentExprNode,
  CallExprNode,
  UnaryExprNode,
  OpExprNode,
  StatNode,
  ExprStatNode,
  BlockStatNode,
  DeclStatNode,
  IfStatNode,
  WhileStatNode,
  BreakStatNode,
  ContinueStatNode,
  ReturnStatNode,
  FuncNode,
  FuncParamNode,
  ProgNode,
};

class Node {
 public:
  NodeType _ntype;
  Node() : _ntype(NodeType::Node) {}
  virtual ~Node() = default;
};

typedef std::shared_ptr<Node> NodePtr;

}  // namespace miniplc0