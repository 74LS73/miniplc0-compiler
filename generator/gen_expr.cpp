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
  auto var = std::dynamic_pointer_cast<DeclStatNode>(assign->_lhs->_var);
  generateGetVariable(var->_id, var->_vscope);
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
  generateStackAlloc(call_node->_return_slots);
  for (auto &expr : call_node->_params) {
    generateExpr(expr);
  }

  if (call_node->_is_std) {
    FuncNodePtr func = std::dynamic_pointer_cast<FuncNode>(call_node->_func);
    generateCallName(func->_global_index);

  } else {
    auto func = std::dynamic_pointer_cast<FuncNode>(call_node->_func);
    generateCallFunction(func->_id);
  }
}

void Generator::generateItemExpr(ItemExprNodePtr item_node) {
  if (item_node->_is_string == true) {
    auto var = std::dynamic_pointer_cast<DeclStatNode>(item_node->_var);
    generateLiteralValue(&(var->_id));
  } else {
    generateLiteralValue(item_node->_value);
  }
}

void Generator::generateIdentExpr(IdentExprNodePtr ident_node) {
  auto var = std::dynamic_pointer_cast<DeclStatNode>(ident_node->_var);
  generateLoadVariable(var->_id, var->_vscope);
}
}  // namespace miniplc0
