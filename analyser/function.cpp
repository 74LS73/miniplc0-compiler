#include "analyser.h"

namespace miniplc0 {

// function -> 'fn' IDENT '(' function_param_list? ')' '->' ty block_stmt
// //               ^~~~      ^~~~~~~~~~~~~~~~~~~~          ^~ ^~~~~~~~~~
// //               |              |                        |  |
// //               function_name  param_list     return_type  function_body
std::optional<CompilationError> Analyser::analyseFunction() {
  std::optional<miniplc0::CompilationError> err;
  auto func = FunctionItem();
  auto next = nextToken();
  if (!next.has_value() || next.value().GetType() != TokenType::FN) {
    return std::make_optional<CompilationError>(
        _current_pos, ErrorCode::ErrNeedDeclareSymbol);
  }

  // Identifier
  next = nextToken();

  if (!next.has_value() || next.value().GetType() != TokenType::IDENTIFIER) {
    return std::make_optional<CompilationError>(_current_pos,
                                                ErrorCode::ErrNeedIdentifier);
  }

  auto fn_token = next.value();
  std::string fn_name = next.value().GetValueString();
  if (isFunctionDeclared(fn_name)) {
    return std::make_optional<CompilationError>(
        _current_pos, ErrorCode::ErrDuplicateDeclaration);
  }
  func.name = fn_name;

  // (
  next = nextToken();
  if (!next.has_value() || next.value().GetType() != TokenType::LEFT_BRACKET) {
    return std::make_optional<CompilationError>(_current_pos,
                                                ErrorCode::ErrNeedBracket);
  }

  // 判断一下是否有参数
  next = nextToken();
  if (next.has_value() && next.value().GetType() == TokenType::RIGHT_BRACKET) {
    goto ARROW;
  }
  unreadToken();

  // 进入SubScope
  pushNextScope();
  err = analyseFunctionParamList(func);
  if (err.has_value()) return err;

  // )
  next = nextToken();
  if (!next.has_value() || next.value().GetType() != TokenType::RIGHT_BRACKET) {
    return std::make_optional<CompilationError>(_current_pos,
                                                ErrorCode::ErrNeedBracket);
  }

  ARROW:
  // ->
  next = nextToken();
  if (!next.has_value() || next.value().GetType() != TokenType::ARROW) {
    return std::make_optional<CompilationError>(_current_pos,
                                                ErrorCode::ErrNeedArrow);
  }

  // return_type
  next = nextToken();
  if (!next.has_value() || !next.value().isTokenAType()) {
    return std::make_optional<CompilationError>(_current_pos,
                                                ErrorCode::ErrNeedType);
  }
  func.return_type = next.value().GetType();
  if (next.value().GetType() != TokenType::VOID) {
    func.return_slots++;
  }


  // body
  err = analyseBlockStatement(func);
  if (err.has_value()) return err;

  // 退出SubScope
  popCurrentScope();

  // TODO
  declareFunction(fn_token, func);

  if (err.has_value()) return err;

  return {};
}

// function_param -> 'const'? IDENT ':' ty
std::optional<CompilationError> Analyser::analyseFunctionParameter(
    FunctionItem &func) {
  std::optional<miniplc0::CompilationError> err;
  auto item = VariableItem();

  auto next = nextToken();
  if (next.has_value() && next.value().GetType() == TokenType::CONST) {
    item.is_const = true;
    next = nextToken();
  }

  auto var_token = next.value();
  if (!next.has_value() || next.value().GetType() != TokenType::IDENTIFIER) {
    return std::make_optional<CompilationError>(_current_pos,
                                                ErrorCode::ErrNeedIdentifier);
  }

  if (isLocalVariableDeclared(next.value().GetValueString())) {
    return std::make_optional<CompilationError>(
        _current_pos, ErrorCode::ErrDuplicateDeclaration);
  }
  
  next = nextToken();
  if (!next.has_value() || next.value().GetType() != TokenType::COLON) {
    return std::make_optional<CompilationError>(_current_pos,
                                                ErrorCode::ErrNeedColon);
  }

  next = nextToken();
  
  if (!next.has_value() || !next.value().isTokenAType()) {
    return std::make_optional<CompilationError>(_current_pos,
                                                ErrorCode::ErrNeedType);
  }
  item.type = next.value().GetType();
  
  declareVariable(var_token, item);

  func.params.push_back(item);
  func.param_slots++;

  return {};
}

// function_param_list -> function_param (',' function_param)*
std::optional<CompilationError> Analyser::analyseFunctionParamList(
    FunctionItem &func) {
  std::optional<miniplc0::CompilationError> err;

  std::optional<miniplc0::Token> next;
  while (true) {
    err = analyseFunctionParameter(func);
    if (err.has_value()) return err;
    next = nextToken();
    if (!next.has_value() || next.value().GetType() != TokenType::COMMA) {
      unreadToken();
      break;
    }
  }

  return {};
}

}  // namespace miniplc0