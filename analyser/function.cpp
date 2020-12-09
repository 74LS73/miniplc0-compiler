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
  if (_symbol_table_stack.isFunctionDeclared(fn_name)) {
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

  // 进入SubScope
  _symbol_table_stack.pushNextScope();

  // 判断一下是否有参数
  next = nextToken();
  if (next.has_value() && next.value().GetType() == TokenType::RIGHT_BRACKET) {
    goto ARROW;
  }
  unreadToken();

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
  func.need_return = false;
  if (next.value().GetType() != TokenType::VOID) {
    func.return_slots++;
    func.need_return = true;
  }

  // body
  
  err = analyseBlockStatement(func, func.need_return);
  if (err.has_value()) return err;

  // 退出SubScope
  _symbol_table_stack.popCurrentScope();
  
  // _symbol_table_stack.getCurrentScopeLevel();
  // TODO
  if (func.need_return) {
    return std::make_optional<CompilationError>(_current_pos,
                                                ErrorCode::ErrNeedReturn);
  }
  _symbol_table_stack.declareFunction(fn_token, func);

  if (err.has_value()) return err;

  return {};
}

// function_param -> 'const'? IDENT ':' ty
std::optional<CompilationError> Analyser::analyseFunctionParameter(
    FunctionItem &func) {
  std::optional<miniplc0::CompilationError> err;
  auto var = VariableItem();
  var.vt = VariableType::PARAM;
  var.id = func.param_slots;

  auto next = nextToken();
  if (next.has_value() && next.value().GetType() == TokenType::CONST) {
    var.is_const = true;
    next = nextToken();
  }

  auto var_token = next.value();
  if (!next.has_value() || next.value().GetType() != TokenType::IDENTIFIER) {
    return std::make_optional<CompilationError>(_current_pos,
                                                ErrorCode::ErrNeedIdentifier);
  }

  if (_symbol_table_stack.isLocalVariableDeclared(
          next.value().GetValueString())) {
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
  var.type = next.value().GetType();

  _symbol_table_stack.declareVariable(var_token, var);

  func.params.push_back(var);
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