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
  _code_stack.push(vector<Instruction>());

  for (auto &stat_node : block_node->_stats) {
    generateStat(stat_node);
  }

  auto block = _code_stack.top();
  _code_stack.pop();
  auto &base_block = _code_stack.top();
  for (auto isa : block) {
    base_block.push_back(isa);
  }
}

void Generator::generateDeclStat(DeclStatNodePtr decl) {
  if (decl->_value == nullptr) return;
  generateGetVariable(decl->_id, decl->_vscope);
  generateExpr(decl->_value);
  generateStore();
}

void Generator::generateIfStat(IfStatNodePtr if_node) {
  _code_stack.push(vector<Instruction>());
  generateExpr(if_node->_expr);

  _code_stack.push(vector<Instruction>());
  generateBlockStat(if_node->_if_block);
  auto if_block = _code_stack.top();
  _code_stack.pop();
  auto &expr_block = _code_stack.top();
  generateBrTrue(1);
  generateBr(if_block.size() + 1);
  for (auto &isa : if_block) {
    expr_block.emplace_back(isa);
  }

  if (if_node->_else_block != nullptr) {
    _code_stack.push(vector<Instruction>());
    generateStat(if_node->_else_block);
    

    auto else_block = _code_stack.top();
    _code_stack.pop();
    generateBr(else_block.size() + 1);
    auto &base_block = _code_stack.top();
    for (auto &isa : else_block) {
      base_block.emplace_back(isa);
    }
  }
  generateBr(0);

  auto block = _code_stack.top();
  _code_stack.pop();
  auto &base_block = _code_stack.top();
  for (auto &isa : block) {
    base_block.emplace_back(isa);
  }
}

void Generator::generateWhileStat(WhileStatNodePtr while_node) {
  _code_stack.push(vector<Instruction>());
  generateBr(0);
  generateExpr(while_node->_expr);

  _code_stack.push(vector<Instruction>());
  generateBlockStat(while_node->_block);

  auto block = _code_stack.top();
  _code_stack.pop();
  auto &expr_block = _code_stack.top();
  generateBrTrue(1);
  generateBr(block.size() + 1);
  for (auto &isa : block) {
    expr_block.emplace_back(isa);
  }

  generateBr(-expr_block.size());

  auto while_block = _code_stack.top();
  _code_stack.pop();
  auto &_base_block = _code_stack.top();
  for (auto &isa : while_block) {
    _base_block.emplace_back(isa);
  }
}

void Generator::generateBreakStat(BreakStatNodePtr) { generateBreak(); }

void Generator::generateContinueStat(ContinueStatNodePtr) {
  generateContinue();
}

void Generator::generateReturnStat(ReturnStatNodePtr return_node) {
  if (return_node->_expr != nullptr) {
    generateGetVariable(-1, VariableType::PARAM);
    generateExpr(return_node->_expr);
    generateStore();
  }

  generateRet();
}

}  // namespace miniplc0
