#include <climits>

#include "analyser.h"

namespace miniplc0 {

#define UNREACHABLE()

// stmt ->
//       expr_stmt
//     | decl_stmt
//     | if_stmt
//     | while_stmt
//     | return_stmt
//     | block_stmt
//     | empty_stmt
StatNodePtr Analyser::analyseStatement() {
  auto next = nextToken();
  if (!next.has_value()) {
    throw AnalyserError({_current_pos, ErrorCode::ErrNeedSemicolon});
  }
  switch (next.value().GetType()) {
    case TokenType::IF:
      unreadToken();
      return analyseIfStatement();
    case TokenType::WHILE:
      unreadToken();
      return analyseWhileStatement();
    case TokenType::BREAK:
      unreadToken();
      return analyseBreakStatement();
    case TokenType::CONTINUE:
      unreadToken();
      return analyseContinueStatement();
    case TokenType::RETURN:
      unreadToken();
      return analyseReturnStatement();
    case TokenType::LEFT_BRACE:
      unreadToken();
      return analyseBlockStatement();
    case TokenType::LET:
    case TokenType::CONST:
      unreadToken();
      return analyseDeclStatement(VariableType::LOCAL);
    case TokenType::SEMICOLON:
    default:
      unreadToken();
      return analyseExprStatement();
  }

  UNREACHABLE();
  return {};
}

// expr_stmt -> expr ';'
StatNodePtr Analyser::analyseExprStatement() {
  auto node = std::make_shared<ExprStatNode>();
  auto expr = analyseExpression();
  node->_expr = expr;
  auto next = nextToken();
  if (!next.has_value() || next.value().GetType() != TokenType::SEMICOLON) {
    throw AnalyserError({_current_pos, ErrorCode::ErrNeedSemicolon});
  }
  return node;
}

// let_decl_stmt -> 'let' IDENT ':' ty ('=' expr)? ';'
// const_decl_stmt -> 'const' IDENT ':' ty '=' expr ';'
// decl_stmt -> let_decl_stmt | const_decl_stmt
DeclStatNodePtr Analyser::analyseDeclStatement(VariableType vscope) {
  auto node = std::make_shared<DeclStatNode>();
  node->_vscope = vscope;
  auto next = nextToken();

  if (!next.has_value()) {
    throw ErrorCode::ErrNeedDeclareSymbol;
  } else if (next.value().GetType() == TokenType::LET) {
    node->_const = false;
  } else if (next.value().GetType() == TokenType::CONST) {
    node->_const = true;
  } else {
    throw ErrorCode::ErrNeedDeclareSymbol;
  }

  // IDENT
  next = nextToken();
  if (!next.has_value() || next.value().GetType() != TokenType::IDENTIFIER) {
    throw ErrorCode::ErrNeedIdentifier;
  }
  node->_name = next->GetValueString();

  // 在当前层添加identity是否合法
  // 在同一作用域内，一个标识符只能由一个变量或常量使用。
  // 变量或常量的类型不能为 void。
  // 如果存在初始化表达式，其类型应当与变量声明时的类型相同。
  // 常量只能被读取，不能被修改。

  // :
  next = nextToken();
  if (!next.has_value() || next.value().GetType() != TokenType::COLON) {
    throw AnalyserError({_current_pos, ErrorCode::ErrNeedColon});
  }

  // TYPE
  next = nextToken();
  if (!next.has_value() || !(next->GetType() == TokenType::INT ||
                             next->GetType() == TokenType::DOUBLE)) {
    throw AnalyserError({_current_pos, ErrorCode::ErrNeedType});
  }

  node->_type = next.value().GetType();

  // ASSIGN
  next = nextToken();
  if (next.has_value() && next.value().GetType() == TokenType::ASSIGN) {
    auto expr = analyseExpression();
    if (expr->_type != node->_type) {
      throw ErrorCode::ErrInvalidAssignment;
    }
    node->_value = expr;
    next = nextToken();
  } else if (node->_const == true) {
    throw ErrorCode::ErrConstantNeedValue;
  }

  if (!next.has_value() || next.value().GetType() != TokenType::SEMICOLON)
    throw AnalyserError({_current_pos, ErrorCode::ErrNeedSemicolon});

  _symbol_table_stack.declareVariable(node);
  // ++func.local_slots;
  return node;
}

// if_stmt -> 'if' expr block_stmt ('else' (block_stmt | if_stmt))?
// //              ^~~~ ^~~~~~~~~~         ^~~~~~~~~~~~~~~~~~~~~~
// //              |     if_block           else_block
// //              condition
StatNodePtr Analyser::analyseIfStatement() {
  auto node = std::make_shared<IfStatNode>();
  auto next = nextToken();
  if (!next.has_value() || next.value().GetType() != TokenType::IF) {
    throw AnalyserError({_current_pos, ErrorCode::ErrNeedDeclareSymbol});
  }

  auto expr = analyseExpression();
  node->_expr = expr;

  auto if_block = analyseBlockStatement();
  node->_if_block = if_block;

  next = nextToken();
  if (next.has_value() && next.value().GetType() == TokenType::ELSE) {
    next = nextToken();
    if (next.has_value() && next.value().GetType() == TokenType::IF) {
      // else if
      unreadToken();
      auto else_block = analyseIfStatement();
      node->_else_block = else_block;
    } else {
      // else
      unreadToken();
      auto else_block = analyseBlockStatement();
      node->_else_block = else_block;
    }
  } else {
    unreadToken();
  }

  return node;
}

// while_stmt -> 'while' expr block_stmt
// //                    ^~~~ ^~~~~~~~~~while_block
// //                     condition
StatNodePtr Analyser::analyseWhileStatement() {
  auto node = std::make_shared<WhileStatNode>();
  auto next = nextToken();
  if (!next.has_value() || next.value().GetType() != TokenType::WHILE) {
    throw AnalyserError({_current_pos, ErrorCode::ErrNeedDeclareSymbol});
  }

  auto expr = analyseExpression();
  node->_expr = expr;

  auto block = analyseBlockStatement();
  node->_block = std::dynamic_pointer_cast<BlockStatNode>(block);

  return node;
}

// Break_stmt -> 'break'  ';'
StatNodePtr Analyser::analyseBreakStatement() {
  auto next = nextToken();
  if (!next.has_value() || next.value().GetType() != TokenType::BREAK) {
    throw AnalyserError({_current_pos, ErrorCode::ErrNeedReturn});
  }

  next = nextToken();
  if (!next.has_value() || next.value().GetType() != TokenType::SEMICOLON) {
    throw AnalyserError({_current_pos, ErrorCode::ErrNeedSemicolon});
  }

  auto node = std::make_shared<BreakStatNode>();
  return node;
}

// continue_stmt -> 'continue'  ';'
StatNodePtr Analyser::analyseContinueStatement() {
  auto next = nextToken();
  if (!next.has_value() || next.value().GetType() != TokenType::CONTINUE) {
    throw AnalyserError({_current_pos, ErrorCode::ErrNeedReturn});
  }

  next = nextToken();
  if (!next.has_value() || next.value().GetType() != TokenType::SEMICOLON) {
    throw AnalyserError({_current_pos, ErrorCode::ErrNeedSemicolon});
  }
  auto node = std::make_shared<ContinueStatNode>();
  return node;
}

// return_stmt -> 'return' expr? ';'
StatNodePtr Analyser::analyseReturnStatement() {
  auto next = nextToken();
  if (!next.has_value() || next.value().GetType() != TokenType::RETURN) {
    throw AnalyserError({_current_pos, ErrorCode::ErrNeedReturn});
  }
  auto node = std::make_shared<ReturnStatNode>();

  printf("analyse return\n");
  auto &cur_table = _symbol_table_stack.getCurrentTable();
  auto func = cur_table.getCurrentFunction();

  if (func->_return_slots) {
    node->_expr = analyseExpression();
  }

  next = nextToken();
  if (!next.has_value() || next.value().GetType() != TokenType::SEMICOLON) {
    throw AnalyserError({_current_pos, ErrorCode::ErrNeedSemicolon});
  }

  node->_id =
      _symbol_table_stack.getCurrentTable().getCurrentFunction()->_param_slots;
  return node;
}

// block_stmt -> '{' stmt* '}'
BlockStatNodePtr Analyser::analyseBlockStatement() {
  auto node = std::make_shared<BlockStatNode>();
  auto next = nextToken();
  if (!next.has_value() || next.value().GetType() != TokenType::LEFT_BRACE) {
    throw AnalyserError({_current_pos, ErrorCode::ErrNeedBrace});
  }
  // _symbol_table_stack.pushNextScopeWithIndex();
  while (true) {
    next = nextToken();
    if (!next.has_value()) {
      throw AnalyserError({_current_pos, ErrorCode::ErrNeedBrace});
    }
    if (next.value().GetType() == TokenType::RIGHT_BRACE) {
      // _symbol_table_stack.popCurrentScopeWithIndex();
      return node;
    }
    unreadToken();
    auto stat = analyseStatement();
    node->_stats.emplace_back(stat);
  }

  throw AnalyserError({_current_pos, ErrorCode::ErrRecognized});
}

// empty_stmt -> ';'
StatNodePtr Analyser::analyseEmptyStatement() {
  auto next = nextToken();
  if (!next.has_value() || next.value().GetType() != TokenType::SEMICOLON) {
    throw AnalyserError({_current_pos, ErrorCode::ErrNeedSemicolon});
  }
  return {};
}

}  // namespace miniplc0