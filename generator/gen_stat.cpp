#include "ast/ast.h"
#include "generator.h"

namespace miniplc0 {

void Generator::generateStat(StatNodePtr stat) {
  switch (stat->_ntype) {
    case NodeType::ExprStatNode: {
      auto node = std::dynamic_pointer_cast<ExprStatNode>(stat);
      generateExprStat(node);
      break;
    }
    case NodeType::BlockStatNode: {
      auto node = std::dynamic_pointer_cast<BlockStatNode>(stat);
      generateBlockStat(node);
      break;
    }
    case NodeType::DeclStatNode: {
      auto node = std::dynamic_pointer_cast<DeclStatNode>(stat);
      generateDeclStat(node);
      break;
    }
    case NodeType::IfStatNode: {
      auto node = std::dynamic_pointer_cast<IfStatNode>(stat);
      generateIfStat(node);
      break;
    }
    case NodeType::WhileStatNode: {
      auto node = std::dynamic_pointer_cast<WhileStatNode>(stat);
      generateWhileStat(node);
      break;
    }
    case NodeType::BreakStatNode: {
      auto node = std::dynamic_pointer_cast<BreakStatNode>(stat);
      generateBreakStat(node);
      break;
    }
    case NodeType::ContinueStatNode: {
      auto node = std::dynamic_pointer_cast<ContinueStatNode>(stat);
      generateContinueStat(node);
      break;
    }
    case NodeType::ReturnStatNode: {
      auto node = std::dynamic_pointer_cast<ReturnStatNode>(stat);
      generateReturnStat(node);
      break;
    }
    default: {
      printf("error!\n");
      break;
    }
  }
  return;
}

void Generator::generateExprStat(ExprStatNodePtr expr_node) {
  generateExpr(expr_node->_expr);
  return;
}

void Generator::generateBlockStat(BlockStatNodePtr block_node) {
  for (auto &stat_node : block_node->_stats) {
    generateStat(stat_node);
  }
}

void Generator::generateDeclStat(DeclStatNodePtr decl) {
  if (decl->_value == nullptr) return;
  generateGetVariable(decl->_id, decl->_vscope);
  generateExpr(decl->_value);
  generateStore();
}

void Generator::generateIfStat(IfStatNodePtr if_node) {
  generateExpr(if_node->_expr);
  generateBlockStat(if_node->_if_block);
  if (if_node->_else_block != nullptr)
    generateStat(if_node->_else_block);
}

void Generator::generateWhileStat(WhileStatNodePtr while_node) {
  generateExpr(while_node->_expr);
  generateBlockStat(while_node->_block);
}

void Generator::generateBreakStat(BreakStatNodePtr) { generateBreak(); }

void Generator::generateContinueStat(ContinueStatNodePtr) {
  generateContinue();
}

void Generator::generateReturnStat(ReturnStatNodePtr return_node) {
  if (return_node->_expr != nullptr) {
    generateGetVariable(return_node->_id, VariableType::PARAM);
    generateExpr(return_node->_expr);
    generateStore();
  }

  generateRet();
}

void Generator::generateWhileStat(BlockStatNodePtr) {}

}  // namespace miniplc0
