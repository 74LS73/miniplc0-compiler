#include "analyser.h"

namespace miniplc0 {

// function -> 'fn' IDENT '(' function_param_list? ')' '->' ty block_stmt
// //               ^~~~      ^~~~~~~~~~~~~~~~~~~~          ^~ ^~~~~~~~~~
// //               |              |                        |  |
// //               function_name  param_list     return_type  function_body
FuncNodePtr Analyser::analyseFunction() {
  auto node = std::make_shared<FuncNode>();
  auto next = nextToken();
  if (!next.has_value() || next.value().GetType() != TokenType::FN) {
    throw AnalyserError({_current_pos, ErrorCode::ErrNeedDeclareSymbol});
  }

  // Identifier
  next = nextToken();
  if (!next.has_value() || next.value().GetType() != TokenType::IDENTIFIER) {
    throw ErrorCode::ErrNeedBracket;
  }
  node->_name = next->GetValueString();

  // (
  next = nextToken();
  if (!next.has_value() || next.value().GetType() != TokenType::LEFT_BRACKET) {
    throw AnalyserError({_current_pos, ErrorCode::ErrNeedBracket});
  }

  _symbol_table_stack.pushNextScope();

  // 判断一下是否有参数
  next = nextToken();
  if (next.has_value() && next.value().GetType() == TokenType::RIGHT_BRACKET) {
    goto ARROW;
  }
  unreadToken();

  analyseFunctionParamList(node);

  // )
  next = nextToken();
  if (!next.has_value() || next.value().GetType() != TokenType::RIGHT_BRACKET) {
    throw AnalyserError({_current_pos, ErrorCode::ErrNeedBracket});
  }

ARROW:
  // ->
  next = nextToken();
  if (!next.has_value() || next.value().GetType() != TokenType::ARROW) {
    throw AnalyserError({_current_pos, ErrorCode::ErrNeedArrow});
  }

  // return_type
  next = nextToken();
  if (!next.has_value() || !next.value().isTokenAType()) {
    throw AnalyserError({_current_pos, ErrorCode::ErrNeedType});
  }

  node->_return_type = next.value().GetType();

  _symbol_table_stack.declareFunction(node);

  // body
  auto body = analyseBlockStatement();
  node->_body = body;

  _symbol_table_stack.popCurrentScope();

  return node;
}

// function_param -> 'const'? IDENT ':' ty
void Analyser::analyseFunctionParameter(FuncNodePtr &func) {
  auto param = std::make_shared<DeclStatNode>();

  auto next = nextToken();
  param->_const = false;
  if (next.has_value() && next.value().GetType() == TokenType::CONST) {
    param->_const = true;
  } else {
    unreadToken();
  }

  next = nextToken();
  if (!next.has_value() || next.value().GetType() != TokenType::IDENTIFIER) {
    throw ErrorCode::ErrNeedIdentifier;
  }

  param->_name = next->GetValueString();

  next = nextToken();
  if (!next.has_value() || next.value().GetType() != TokenType::COLON) {
    throw AnalyserError({_current_pos, ErrorCode::ErrNeedColon});
  }

  next = nextToken();

  if (!next.has_value() || !next.value().isTokenAType()) {
    throw AnalyserError({_current_pos, ErrorCode::ErrNeedType});
  }
  param->_type = next->GetType();

  _symbol_table_stack.declareVariable(param);

  func->_params.emplace_back(param);

  return;
}

// function_param_list -> function_param (',' function_param)*
void Analyser::analyseFunctionParamList(FuncNodePtr &func) {
  optional<Token> next;
  while (true) {
    analyseFunctionParameter(func);
    next = nextToken();
    if (!next.has_value() || next.value().GetType() != TokenType::COMMA) {
      unreadToken();
      break;
    }
  }
  return;
}

}  // namespace miniplc0