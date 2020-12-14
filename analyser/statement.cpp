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
std::optional<CompilationError> Analyser::analyseStatement(
    FunctionItem &func, bool &need_return, std::shared_ptr<Item> lhs) {
  std::optional<miniplc0::CompilationError> err;
  auto next = nextToken();

  if (!next.has_value()) {
    return std::make_optional<CompilationError>(_current_pos,
                                                ErrorCode::ErrNeedSemicolon);
  }
  switch (next.value().GetType()) {
    case TokenType::IF:
      unreadToken();
      err = analyseIfStatement(func, need_return, lhs);
      if (err.has_value()) return err;
      break;
    case TokenType::WHILE:
      unreadToken();
      err = analyseWhileStatement(func, lhs);
      if (err.has_value()) return err;
      break;
    case TokenType::RETURN:
      unreadToken();
      err = analyseReturnStatement(func, lhs);
      if (err.has_value()) return err;
      need_return = false;
      break;
    case TokenType::LEFT_BRACE:
      unreadToken();
      err = analyseBlockStatement(func, need_return, lhs);
      if (err.has_value()) return err;
      break;
    case TokenType::LET:
      unreadToken();
      err = analyseDeclVariableStatement(func, VariableType::LOCAL, lhs);
      if (err.has_value()) return err;
      break;
    case TokenType::CONST:
      unreadToken();
      err = analyseDeclConstStatement(func, VariableType::LOCAL, lhs);
      if (err.has_value()) return err;
      break;
    case TokenType::SEMICOLON:
      break;
    default:
      unreadToken();
      err = analyseExprStatement(func, lhs);
      if (err.has_value()) return err;
      break;
  }
  return {};
}

// expr_stmt -> expr ';'
std::optional<CompilationError> Analyser::analyseExprStatement(
    FunctionItem &func, std::shared_ptr<Item> lhs) {
  auto err = analyseExpression(lhs);
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
    FunctionItem &func, VariableType vt, std::shared_ptr<Item> lhs) {
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
  var.name = next.value().GetValueString();

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
  if (!next.has_value() || !next.value().isTokenAType()) {
    return std::make_optional<CompilationError>(_current_pos,
                                                ErrorCode::ErrNeedType);
  }
  var.type = next.value().GetType();

  // ASSIGN
  next = nextToken();
  auto expr_code = std::shared_ptr<Item>(new Item());
  if (next.has_value() && next.value().GetType() == TokenType::ASSIGN) {
    
    err = analyseExpression(expr_code);
    if (err.has_value()) return err;
    next = nextToken();
  }
  if (!next.has_value() || next.value().GetType() != TokenType::SEMICOLON)
    return std::make_optional<CompilationError>(_current_pos,
                                                ErrorCode::ErrNeedSemicolon);

  _symbol_table_stack.declareVariable(var);
  ++func.local_slots;

  //Code Gen
  lhs->p_code_gen.generateGetVariable(var.id, var.vt);
  lhs->p_code_gen += expr_code->p_code_gen;
  lhs->p_code_gen.generateStore();
  //END
  return {};
}

// const_decl_stmt -> 'const' IDENT ':' ty '=' expr ';'
// decl_stmt -> let_decl_stmt | const_decl_stmt
std::optional<CompilationError> Analyser::analyseDeclConstStatement(
    FunctionItem &func, VariableType vt, std::shared_ptr<Item> lhs) {
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
  var.name = next.value().GetValueString();


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
  if (!next.has_value() || !next.value().isTokenAType()) {
    return std::make_optional<CompilationError>(_current_pos,
                                                ErrorCode::ErrNeedType);
  }
  var.type = next.value().GetType();

  // ASSIGN
  next = nextToken();
  if (!next.has_value() || next.value().GetType() != TokenType::ASSIGN) {
    return std::make_optional<CompilationError>(
        _current_pos, ErrorCode::ErrConstantNeedValue);
  }

  auto expr_code = std::shared_ptr<Item>(new Item());
  err = analyseExpression(expr_code);
  if (err.has_value()) return err;
  next = nextToken();
  if (!next.has_value() || next.value().GetType() != TokenType::SEMICOLON)
    return std::make_optional<CompilationError>(_current_pos,
                                                ErrorCode::ErrNeedSemicolon);

  _symbol_table_stack.declareVariable(var);
  ++func.local_slots;

  //Code Gen
  lhs->p_code_gen.generateGetVariable(var.id, var.vt);
  lhs->p_code_gen += expr_code->p_code_gen;
  lhs->p_code_gen.generateStore();
  //END
  return {};
}

// if_stmt -> 'if' expr block_stmt ('else' (block_stmt | if_stmt))?
// //              ^~~~ ^~~~~~~~~~         ^~~~~~~~~~~~~~~~~~~~~~
// //              |     if_block           else_block
// //              condition
std::optional<CompilationError> Analyser::analyseIfStatement(
    FunctionItem &func, bool &need_return, std::shared_ptr<Item> lhs) {
  auto next = nextToken();
  std::optional<miniplc0::CompilationError> err;
  if (!next.has_value() || next.value().GetType() != TokenType::IF) {
    return std::make_optional<CompilationError>(
        _current_pos, ErrorCode::ErrNeedDeclareSymbol);
  }

  err = analyseExpression(lhs);
  if (err.has_value()) return err;

  auto if_gen = std::shared_ptr<miniplc0::Item>(new Item());
  bool if_need_return = true && need_return;
  err = analyseBlockStatement(func, if_need_return, if_gen);
  if (err.has_value()) return err;


  auto else_gen = std::shared_ptr<miniplc0::Item>(new Item());
  bool else_need_return = true && need_return;
  next = nextToken();
  if (next.has_value() && next.value().GetType() == TokenType::ELSE) {
    next = nextToken();
    if (next.has_value() && next.value().GetType() == TokenType::IF) {
      // else if
      unreadToken();
      err = analyseIfStatement(func, else_need_return, else_gen);
      if (err.has_value()) return err;
    } else {
      // else
      unreadToken();
      analyseBlockStatement(func, else_need_return, else_gen);
      if (err.has_value()) return err;
    }
    else_gen->p_code_gen.generateBr(0);
  } else {
    unreadToken();
  }

  // IF_ELSE
  lhs->p_code_gen.generateBrTrue(1);
  lhs->p_code_gen.generateBr(if_gen->p_code_gen.size());
  lhs->p_code_gen += if_gen->p_code_gen;
  lhs->p_code_gen += else_gen->p_code_gen;
  // END

  need_return = if_need_return || else_need_return;
  return {};
}

// while_stmt -> 'while' expr block_stmt
// //                    ^~~~ ^~~~~~~~~~while_block
// //                     condition
std::optional<CompilationError> Analyser::analyseWhileStatement(
    FunctionItem &func, std::shared_ptr<Item> lhs) {
  auto next = nextToken();
  std::optional<miniplc0::CompilationError> err;
  if (!next.has_value() || next.value().GetType() != TokenType::WHILE) {
    return std::make_optional<CompilationError>(
        _current_pos, ErrorCode::ErrNeedDeclareSymbol);
  }

  err = analyseExpression(lhs);
  if (err.has_value()) return err;

  bool while_need_return = false;
  err = analyseBlockStatement(func, while_need_return, lhs);
  if (err.has_value()) return err;

  return {};
}

// return_stmt -> 'return' expr? ';'
std::optional<CompilationError> Analyser::analyseReturnStatement(
    FunctionItem &func, std::shared_ptr<Item> lhs) {
  auto next = nextToken();
  std::optional<miniplc0::CompilationError> err;
  if (!next.has_value() || next.value().GetType() != TokenType::RETURN) {
    return std::make_optional<CompilationError>(_current_pos,
                                                ErrorCode::ErrNeedReturn);
  }

  if (func.return_slots) {
    lhs->p_code_gen.generateGetVariable(0, VariableType::PARAM);
    err = analyseExpression(lhs);
    if (err.has_value()) return err;
    lhs->p_code_gen.generateStore();
    lhs->p_code_gen.generateRet();
  }

  next = nextToken();
  if (!next.has_value() || next.value().GetType() != TokenType::SEMICOLON) {
    return std::make_optional<CompilationError>(_current_pos,
                                                ErrorCode::ErrNeedSemicolon);
  }

  return {};
}

// block_stmt -> '{' stmt* '}'
std::optional<CompilationError> Analyser::analyseBlockStatement(
    FunctionItem &func, bool &need_return, std::shared_ptr<Item> lhs) {
  auto next = nextToken();
  std::optional<miniplc0::CompilationError> err;
  if (!next.has_value() || next.value().GetType() != TokenType::LEFT_BRACE) {
    return std::make_optional<CompilationError>(_current_pos,
                                                ErrorCode::ErrNeedBrace);
  }
  while (true) {
    err = analyseStatement(func, need_return, lhs);
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