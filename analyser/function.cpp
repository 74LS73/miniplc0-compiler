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
  auto ident = analyseIdentExpression();
  node->_ident = ident;

  // (
  next = nextToken();
  if (!next.has_value() || next.value().GetType() != TokenType::LEFT_BRACKET) {
    throw AnalyserError({_current_pos, ErrorCode::ErrNeedBracket});
  }

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

  // body
  auto body = analyseBlockStatement();
  node->_body = body;

  return node;
}

// function_param -> 'const'? IDENT ':' ty
void Analyser::analyseFunctionParameter(FuncNodePtr &func) {
  auto next = nextToken();
  bool is_const = false;
  if (next.has_value() && next.value().GetType() == TokenType::CONST) {
    is_const = true;
  } else {
    unreadToken();
  }

  auto ident = analyseIdentExpression();
  auto param = std::make_shared<FuncParamNode>();
  param->_ident = ident;
  param->_const = is_const;

  next = nextToken();
  if (!next.has_value() || next.value().GetType() != TokenType::COLON) {
    throw AnalyserError({_current_pos, ErrorCode::ErrNeedColon});
  }

  next = nextToken();

  if (!next.has_value() || !next.value().isTokenAType()) {
    throw AnalyserError({_current_pos, ErrorCode::ErrNeedType});
  }
  param->_type = next->GetType();
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