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
      printf("error!");
      break;
    }
  }
  return;
}

void Generator::generateAssignExpr(AssignExprNodePtr assign) {
  generateLoadVariable(assign->_lhs->_id, VariableType::PARAM);
  generateExpr(assign->_rhs);
  generateStore();
}

void Generator::generateUnaryExpr(UnaryExprNodePtr unary) {
  generateExpr(unary->_ident);
  if (unary->_nega) {
    generateNega(unary->_ident->_type);
  }
  if (unary->_ident->_type != unary->_type) {
    generateAs(unary->_ident->_type, unary->_type);
  }
}

void Generator::generateOpExpr(OpExprNodePtr) {

}

void Generator::generateCallExpr(CallExprNodePtr call) {
  generateCallFunction(call->_id);
}

}  // namespace miniplc0
