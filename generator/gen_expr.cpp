#include "ast/ast.h"
#include "generator.h"

namespace miniplc0 {

void Generator::generateExpr(ExprNodePtr expr) {
  switch (expr->_ntype) {
    case NodeType::AssignExprNode: {
      auto node = std::dynamic_pointer_cast<AssignExprNode>(expr);
      generateAssignExpr(node);
      break;
    }
    case NodeType::ItemExprNode: {
      auto node = std::dynamic_pointer_cast<ItemExprNode>(expr);
      generateItemExpr(node);
      break;
    }
    case NodeType::IdentExprNode: {
      auto node = std::dynamic_pointer_cast<IdentExprNode>(expr);
      generateIdentExpr(node);
      break;
    }
    case NodeType::UnaryExprNode: {
      auto node = std::dynamic_pointer_cast<UnaryExprNode>(expr);
      generateUnaryExpr(node);
      break;
    }
    case NodeType::OpExprNode: {
      auto node = std::dynamic_pointer_cast<OpExprNode>(expr);
      generateOpExpr(node);
      break;
    }
    case NodeType::CallExprNode: {
      auto node = std::dynamic_pointer_cast<CallExprNode>(expr);
      generateCallExpr(node);
      break;
    }
    default: {
      printf("ntype is %d !", expr->_ntype);
      printf("error!\n");
      break;
    }
  }
  return;
}

void Generator::generateAssignExpr(AssignExprNodePtr assign) {
  generateGetVariable(assign->_lhs->_id, assign->_lhs->_vscope);
  generateExpr(assign->_rhs);
  generateStore();
}

void Generator::generateUnaryExpr(UnaryExprNodePtr unary) {
  generateExpr(unary->_item);
  if (unary->_nega) {
    generateNega(unary->_item->_type);
  }
  if (unary->_item->_type != unary->_type) {
    generateAs(unary->_item->_type, unary->_type);
  }
}

void Generator::generateOpExpr(OpExprNodePtr op_node) {
  // if (op_node->_lhs == nullptr) {
  //   generateUnaryExpr(op_node);
  // }
  generateExpr(op_node->_lhs);
  if (op_node->_rhs != nullptr) {
    generateExpr(op_node->_rhs);
    generateOperation(op_node->_type, op_node->_operator);
  }
}

void Generator::generateCallExpr(CallExprNodePtr call_node) {
  generateStackAlloc(1);
  for (auto &expr : call_node->_params) {
    generateExpr(expr);
  }
  generateCallFunction(call_node->_id);
}

void Generator::generateItemExpr(ItemExprNodePtr item_node) {
  generateLiteralValue(item_node->_value);
}

void Generator::generateIdentExpr(IdentExprNodePtr ident_node) {
  generateLoadVariable(ident_node->_id, ident_node->_vscope);
}
}  // namespace miniplc0
