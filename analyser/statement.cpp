#include <climits>

#include "analyser.h"

namespace miniplc0 {

// stmt ->
//       expr_stmt
//     | decl_stmt
//     | if_stmt
//     | while_stmt
//     | return_stmt
//     | block_stmt
//     | empty_stmt
std::optional<CompilationError> Analyser::analyseStatement(FunctionItem &func, bool &need_return) {
  std::optional<miniplc0::CompilationError> err;
  auto next = nextToken();

  if (!next.has_value()) {
    return std::make_optional<CompilationError>(_current_pos,
                                                ErrorCode::ErrNeedSemicolon);
  }
  switch (next.value().GetType()) {
    case TokenType::IF:
      unreadToken();
      err = analyseIfStatement(func, need_return);
      if (err.has_value()) return err;
      break;
    case TokenType::WHILE:
      unreadToken();
      err = analyseWhileStatement(func);
      if (err.has_value()) return err;
      break;
    case TokenType::RETURN:
      unreadToken();
      err = analyseReturnStatement(func);
      if (err.has_value()) return err;
      need_return = false;
      break;
    case TokenType::LEFT_BRACE:
      unreadToken();
      err = analyseBlockStatement(func, need_return);
      if (err.has_value()) return err;
      break;
    case TokenType::LET:
      unreadToken();
      err = analyseDeclVariableStatement(func, VariableType::LOCAL);
      if (err.has_value()) return err;
      break;
    case TokenType::CONST:
      unreadToken();
      err = analyseDeclConstStatement(func, VariableType::LOCAL);
      if (err.has_value()) return err;
      break;
    case TokenType::SEMICOLON:
      break;
    default:
      unreadToken();
      err = analyseExprStatement(func);
      if (err.has_value()) return err;
      break;
  }
  return {};
}

// expr_stmt -> expr ';'
std::optional<CompilationError> Analyser::analyseExprStatement(
    FunctionItem &func) {
  auto err = analyseExpression(func);
  if (err.has_value()) return err;

  auto next = nextToken();
  if (!next.has_value() || next.value().GetType() != TokenType::SEMICOLON) {
    return std::make_optional<CompilationError>(_current_pos,
                                                ErrorCode::ErrNeedSemicolon);
  }
  return {};
}

// let_decl_stmt -> 'let' IDENT ':' ty ('=' expr)? ';'
std::optional<CompilationError> Analyser::analyseDeclVariableStatement(
    FunctionItem &func, VariableType vt) {
  // 未初始化统一为0
  auto var = VariableItem();
  var.vt = vt;
  var.id = func.local_slots;

  auto next = nextToken();
  std::optional<miniplc0::CompilationError> err;
  if (!next.has_value() || next.value().GetType() != TokenType::LET) {
    return std::make_optional<CompilationError>(
        _current_pos, ErrorCode::ErrNeedDeclareSymbol);
  }
  var.is_const = false;

  // IDENT
  next = nextToken();
  if (!next.has_value() || next.value().GetType() != TokenType::IDENTIFIER) {
    return std::make_optional<CompilationError>(_current_pos,
                                                ErrorCode::ErrNeedIdentifier);
  }
  auto var_token = next.value();
  if (_symbol_table_stack.isLocalVariableDeclared(
          next.value().GetValueString())) {
    return std::make_optional<CompilationError>(
        _current_pos, ErrorCode::ErrDuplicateDeclaration);
  }

  // 在当前层添加identity是否合法
  // 在同一作用域内，一个标识符只能由一个变量或常量使用。
  // 变量或常量的类型不能为 void。
  // 如果存在初始化表达式，其类型应当与变量声明时的类型相同。
  // 常量只能被读取，不能被修改。
  // pushTokenToCurrentTable();

  // :
  next = nextToken();
  if (!next.has_value() || next.value().GetType() != TokenType::COLON) {
    return std::make_optional<CompilationError>(_current_pos,
                                                ErrorCode::ErrNeedColon);
  }

  // TYPE
  next = nextToken();
  if (!next.has_value() || next.value().GetType() != TokenType::INT ||
      next.value().GetType() != TokenType::DOUBLE) {
    // return std::make_optional<CompilationError>(_current_pos,
    //                                          ErrorCode::ErrNeedType);
  }
  var.type = next.value().GetType();

  // ASSIGN
  next = nextToken();
  if (next.has_value() && next.value().GetType() == TokenType::ASSIGN) {
    err = analyseExpression(func);
    if (err.has_value()) return err;
    next = nextToken();
  }
  if (!next.has_value() || next.value().GetType() != TokenType::SEMICOLON)
    return std::make_optional<CompilationError>(_current_pos,
                                                ErrorCode::ErrNeedSemicolon);

  _symbol_table_stack.declareVariable(var_token, var);
  ++func.local_slots;

  return {};
}

// const_decl_stmt -> 'const' IDENT ':' ty '=' expr ';'
// decl_stmt -> let_decl_stmt | const_decl_stmt
std::optional<CompilationError> Analyser::analyseDeclConstStatement(
    FunctionItem &func, VariableType vt) {
  auto var = VariableItem();
  var.vt = vt;
  var.id = func.local_slots;

  auto next = nextToken();
  std::optional<miniplc0::CompilationError> err;
  if (!next.has_value() || next.value().GetType() != TokenType::CONST) {
    return std::make_optional<CompilationError>(
        _current_pos, ErrorCode::ErrNeedDeclareSymbol);
  }
  var.is_const = true;

  // IDENT
  next = nextToken();
  if (!next.has_value() || next.value().GetType() != TokenType::IDENTIFIER) {
    return std::make_optional<CompilationError>(_current_pos,
                                                ErrorCode::ErrNeedIdentifier);
  }
  auto var_token = next.value();
  if (_symbol_table_stack.isLocalVariableDeclared(
          next.value().GetValueString())) {
    return std::make_optional<CompilationError>(
        _current_pos, ErrorCode::ErrDuplicateDeclaration);
  }

  // :
  next = nextToken();
  if (!next.has_value() || next.value().GetType() != TokenType::COLON) {
    return std::make_optional<CompilationError>(_current_pos,
                                                ErrorCode::ErrNeedColon);
  }
  // TYPE
  next = nextToken();
  if (!next.has_value() || (next.value().GetType() != TokenType::INT &&
                            next.value().GetType() != TokenType::DOUBLE)) {
    // return std::make_optional<CompilationError>(_current_pos,
    //                                          ErrorCode::ErrNeedType);
  }
  var.type = next.value().GetType();

  // ASSIGN
  next = nextToken();
  if (!next.has_value() || next.value().GetType() != TokenType::ASSIGN) {
    return std::make_optional<CompilationError>(
        _current_pos, ErrorCode::ErrConstantNeedValue);
  }
  err = analyseExpression(func);
  if (err.has_value()) return err;
  next = nextToken();
  if (!next.has_value() || next.value().GetType() != TokenType::SEMICOLON)
    return std::make_optional<CompilationError>(_current_pos,
                                                ErrorCode::ErrNeedSemicolon);

  _symbol_table_stack.declareVariable(var_token, var);
  ++func.local_slots;

  return {};
}

// if_stmt -> 'if' expr block_stmt ('else' (block_stmt | if_stmt))?
// //              ^~~~ ^~~~~~~~~~         ^~~~~~~~~~~~~~~~~~~~~~
// //              |     if_block           else_block
// //              condition
std::optional<CompilationError> Analyser::analyseIfStatement(
    FunctionItem &func, bool &need_return) {
  auto next = nextToken();
  std::optional<miniplc0::CompilationError> err;
  if (!next.has_value() || next.value().GetType() != TokenType::IF) {
    return std::make_optional<CompilationError>(
        _current_pos, ErrorCode::ErrNeedDeclareSymbol);
  }
  err = analyseExpression(func);
  if (err.has_value()) return err;

  bool if_need_return = true && need_return;
  err = analyseBlockStatement(func, if_need_return);
  if (err.has_value()) return err;


  bool else_need_return = true && need_return;
  next = nextToken();
  if (next.has_value() && next.value().GetType() == TokenType::ELSE) {
    next = nextToken();
    if (next.has_value() && next.value().GetType() == TokenType::IF) {
      // else if
      unreadToken();
      err = analyseIfStatement(func, else_need_return);
      if (err.has_value()) return err;
    } else {
      // else
      unreadToken();
      analyseBlockStatement(func, else_need_return);
      if (err.has_value()) return err;
    }
  } else {
    unreadToken();
  }

  need_return = if_need_return || else_need_return;
  return {};
}

// while_stmt -> 'while' expr block_stmt
// //                    ^~~~ ^~~~~~~~~~while_block
// //                     condition
std::optional<CompilationError> Analyser::analyseWhileStatement(
    FunctionItem &func) {
  auto next = nextToken();
  std::optional<miniplc0::CompilationError> err;
  if (!next.has_value() || next.value().GetType() != TokenType::WHILE) {
    return std::make_optional<CompilationError>(
        _current_pos, ErrorCode::ErrNeedDeclareSymbol);
  }
  err = analyseExpression(func);
  if (err.has_value()) return err;

  bool while_need_return = false;
  err = analyseBlockStatement(func, while_need_return);
  if (err.has_value()) return err;

  return {};
}

// return_stmt -> 'return' expr? ';'
std::optional<CompilationError> Analyser::analyseReturnStatement(
    FunctionItem &func) {
  auto next = nextToken();
  std::optional<miniplc0::CompilationError> err;
  if (!next.has_value() || next.value().GetType() != TokenType::RETURN) {
    return std::make_optional<CompilationError>(_current_pos,
                                                ErrorCode::ErrNeedReturn);
  }

  next = nextToken();
  if (!next.has_value() || next.value().GetType() != TokenType::SEMICOLON) {
    unreadToken();
    err = analyseExpression(func);
    if (err.has_value()) return err;
    next = nextToken();
  }
  if (next.has_value() && next.value().GetType() == TokenType::SEMICOLON) {
    return {};
  } else {
    return std::make_optional<CompilationError>(_current_pos,
                                                ErrorCode::ErrNeedSemicolon);
  }
}

// block_stmt -> '{' stmt* '}'
std::optional<CompilationError> Analyser::analyseBlockStatement(
    FunctionItem &func, bool &need_return) {
  auto next = nextToken();
  std::optional<miniplc0::CompilationError> err;
  if (!next.has_value() || next.value().GetType() != TokenType::LEFT_BRACE) {
    return std::make_optional<CompilationError>(_current_pos,
                                                ErrorCode::ErrNeedBrace);
  }
  while (true) {
    err = analyseStatement(func, need_return);
    if (err.has_value()) return err;
    next = nextToken();
    if (next.has_value() && next.value().GetType() == TokenType::RIGHT_BRACE) {
      return {};
    }
    unreadToken();
  }

  return std::make_optional<CompilationError>(_current_pos,
                                              ErrorCode::ErrRecognized);
}

// empty_stmt -> ';'
std::optional<CompilationError> Analyser::analyseEmptyStatement() {
  auto next = nextToken();
  std::optional<miniplc0::CompilationError> err;
  if (!next.has_value() || next.value().GetType() != TokenType::SEMICOLON) {
    return std::make_optional<CompilationError>(_current_pos,
                                                ErrorCode::ErrNeedSemicolon);
  }

  return {};
}

}  // namespace miniplc0