#include "analyser.h"

namespace miniplc0 {

// function -> 'fn' IDENT '(' function_param_list? ')' '->' ty block_stmt
// //               ^~~~      ^~~~~~~~~~~~~~~~~~~~          ^~ ^~~~~~~~~~
// //               |              |                        |  |
// //               function_name  param_list     return_type  function_body
FuncNodePtr Analyser::analyseFunction() {
  printf("sdfsdf");
  auto node = std::make_shared<FuncNode>();
  auto next = nextToken();
  if (!next.has_value() || next.value().GetType() != TokenType::FN) {
    throw ErrorCode::ErrNeedDeclareSymbol;
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
    throw ErrorCode::ErrNeedBracket;
  }

  _symbol_table_stack.pushNextScope();

  // 判断一下是否有参数
  next = nextToken();
  if (next.has_value() && next.value().GetType() == TokenType::RIGHT_BRACKET) {
    node->_param_slots = 0;
    goto ARROW;
  }
  unreadToken();

  analyseFunctionParamList(node);

  node->_param_slots = node->_params.size();
  // )
  next = nextToken();
  if (!next.has_value() || next.value().GetType() != TokenType::RIGHT_BRACKET) {
    throw ErrorCode::ErrNeedBracket;
  }

ARROW:
  // ->
  
  next = nextToken();
  if (!next.has_value() || next.value().GetType() != TokenType::ARROW) {
    throw ErrorCode::ErrNeedArrow;
  }

  // return_type
  next = nextToken();
  if (!next.has_value() || !next.value().isTokenAType()) {
    throw ErrorCode::ErrNeedType;
  }

  node->_return_type = next.value().GetType();
  if (node->_return_type != TokenType::VOID) {
    node->_return_slots = 1;
  } else {
    node->_return_slots = 0;
  }

  _symbol_table_stack.declareFunction(node);

  // body
  printf("sdfsdf");
  auto body = analyseBlockStatement();
  node->_body = body;

  node->_loc_slots = _symbol_table_stack.getCurrentVariableNumber();

  _symbol_table_stack.popCurrentScope();

  return node;
}

// function_param -> 'const'? IDENT ':' ty
void Analyser::analyseFunctionParameter(FuncNodePtr &func) {
  auto param = std::make_shared<DeclStatNode>();
  param->_vscope = VariableType::PARAM;
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
    throw ErrorCode::ErrNeedColon;
  }

  next = nextToken();

  if (!next.has_value() || !next.value().isTokenAType()) {
    throw ErrorCode::ErrNeedType;
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