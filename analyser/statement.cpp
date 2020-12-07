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
std::optional<CompilationError> Analyser::analyseStatement() {
  std::optional<miniplc0::CompilationError> err;
  auto next = nextToken();

  if (!next.has_value()) {
    return std::make_optional<CompilationError>(_current_pos,
                                                ErrorCode::ErrNeedSemicolon);
  }
  switch (next.value().GetType()) {
    case TokenType::IF:
      unreadToken();
      err = analyseIfStatement();
      if (err.has_value()) return err;
      break;
    case TokenType::WHILE:
      unreadToken();
      err = analyseWhileStatement();
      if (err.has_value()) return err;
      break;
    case TokenType::RETURN:
      unreadToken();
      err = analyseReturnStatement();
      if (err.has_value()) return err;
      break;
    case TokenType::LEFT_BRACE:
      unreadToken();
      err = analyseBlockStatement();
      if (err.has_value()) return err;
      break;
    case TokenType::LET:
      unreadToken();
      err = analyseDeclVariableStatement();
      if (err.has_value()) return err;
      break;
    case TokenType::CONST:
      unreadToken();
      err = analyseDeclConstStatement();
      if (err.has_value()) return err;
      break;
    case TokenType::SEMICOLON:
      break;
    default:
      unreadToken();
      err = analyseExprStatement();
      if (err.has_value()) return err;
      break;
  }
  return {};
}

// expr_stmt -> expr ';'
std::optional<CompilationError> Analyser::analyseExprStatement() {
  auto err = analyseExpression();
  if (err.has_value()) return err;

  auto next = nextToken();
  if (!next.has_value() || next.value().GetType() != TokenType::SEMICOLON) {
    return std::make_optional<CompilationError>(_current_pos,
                                                ErrorCode::ErrNeedSemicolon);
  }
  return {};
}

// let_decl_stmt -> 'let' IDENT ':' ty ('=' expr)? ';'
std::optional<CompilationError> Analyser::analyseDeclVariableStatement() {
  // 未初始化统一为0
  auto item = VariableItem();
  auto next = nextToken();
  std::optional<miniplc0::CompilationError> err;
  if (!next.has_value() || next.value().GetType() != TokenType::LET) {
    return std::make_optional<CompilationError>(
        _current_pos, ErrorCode::ErrNeedDeclareSymbol);
  }

  // IDENT
  next = nextToken();
  if (!next.has_value() || next.value().GetType() != TokenType::IDENTIFIER) {
    return std::make_optional<CompilationError>(_current_pos,
                                                ErrorCode::ErrNeedIdentifier);
  }
  auto var_token = next.value();
  if (isLocalVariableDeclared(next.value().GetValueString())) {
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
  item.type = next.value().GetType();

  // ASSIGN
  next = nextToken();
  if (next.has_value() && next.value().GetType() == TokenType::ASSIGN) {
    err = analyseExpression();
    if (err.has_value()) return err;
    next = nextToken();
  }
  if (!next.has_value() || next.value().GetType() != TokenType::SEMICOLON)
    return std::make_optional<CompilationError>(_current_pos,
                                                ErrorCode::ErrNeedSemicolon);

  declareVariable(var_token, item);

  return {};
}

// const_decl_stmt -> 'const' IDENT ':' ty '=' expr ';'
// decl_stmt -> let_decl_stmt | const_decl_stmt
std::optional<CompilationError> Analyser::analyseDeclConstStatement() {
  auto item = VariableItem();
  auto next = nextToken();
  std::optional<miniplc0::CompilationError> err;
  if (!next.has_value() || next.value().GetType() != TokenType::CONST) {
    return std::make_optional<CompilationError>(
        _current_pos, ErrorCode::ErrNeedDeclareSymbol);
  }
  item.is_const = true;

  // IDENT
  next = nextToken();
  if (!next.has_value() || next.value().GetType() != TokenType::IDENTIFIER) {
    return std::make_optional<CompilationError>(_current_pos,
                                                ErrorCode::ErrNeedIdentifier);
  }
  auto var_token = next.value();
  if (isLocalVariableDeclared(next.value().GetValueString()) == true) {
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
  item.type = next.value().GetType();

  // ASSIGN
  next = nextToken();
  if (!next.has_value() || next.value().GetType() != TokenType::ASSIGN) {
    return std::make_optional<CompilationError>(
        _current_pos, ErrorCode::ErrConstantNeedValue);
  }
  err = analyseExpression();
  if (err.has_value()) return err;
  next = nextToken();
  if (!next.has_value() || next.value().GetType() != TokenType::SEMICOLON)
    return std::make_optional<CompilationError>(_current_pos,
                                                ErrorCode::ErrNeedSemicolon);
  declareVariable(var_token, item);
  return {};
}

// if_stmt -> 'if' expr block_stmt ('else' (block_stmt | if_stmt))?
// //              ^~~~ ^~~~~~~~~~         ^~~~~~~~~~~~~~~~~~~~~~
// //              |     if_block           else_block
// //              condition
std::optional<CompilationError> Analyser::analyseIfStatement() {
  auto next = nextToken();
  std::optional<miniplc0::CompilationError> err;
  if (!next.has_value() || next.value().GetType() != TokenType::IF) {
    return std::make_optional<CompilationError>(
        _current_pos, ErrorCode::ErrNeedDeclareSymbol);
  }
  err = analyseExpression();
  if (err.has_value()) return err;
  err = analyseBlockStatement();
  if (err.has_value()) return err;

  next = nextToken();
  if (next.has_value() && next.value().GetType() == TokenType::ELSE) {
  } else {
    unreadToken();
  }

  return {};
}

// while_stmt -> 'while' expr block_stmt
// //                    ^~~~ ^~~~~~~~~~while_block
// //                     condition
std::optional<CompilationError> Analyser::analyseWhileStatement() {
  auto next = nextToken();
  std::optional<miniplc0::CompilationError> err;
  if (!next.has_value() || next.value().GetType() != TokenType::WHILE) {
    return std::make_optional<CompilationError>(
        _current_pos, ErrorCode::ErrNeedDeclareSymbol);
  }
  err = analyseExpression();
  if (err.has_value()) return err;
  err = analyseBlockStatement();
  if (err.has_value()) return err;

  next = nextToken();
  if (next.has_value() && next.value().GetType() == TokenType::ELSE) {
    err = analyseBlockStatement();
    if (err.has_value()) return err;
  } else {
    unreadToken();
  }

  return {};
}

// return_stmt -> 'return' expr? ';'
std::optional<CompilationError> Analyser::analyseReturnStatement() {
  auto next = nextToken();
  std::optional<miniplc0::CompilationError> err;
  if (!next.has_value() || next.value().GetType() != TokenType::RETURN) {
    return std::make_optional<CompilationError>(_current_pos,
                                                ErrorCode::ErrNeedReturn);
  }

  next = nextToken();
  if (!next.has_value() || next.value().GetType() != TokenType::SEMICOLON) {
    unreadToken();
    err = analyseExpression();
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
std::optional<CompilationError> Analyser::analyseBlockStatement() {
  auto next = nextToken();
  std::optional<miniplc0::CompilationError> err;
  if (!next.has_value() || next.value().GetType() != TokenType::LEFT_BRACE) {
    return std::make_optional<CompilationError>(_current_pos,
                                                ErrorCode::ErrNeedBrace);
  }
  while (true) {
    err = analyseStatement();
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