#include "analyser.h"

namespace miniplc0{

/*
expr -> 
      operator_expr
    | negate_expr
    | assign_expr
    | as_expr
    | call_expr
    | literal_expr
    | ident_expr
    | group_expr
*/
std::optional<CompilationError> Analyser::analyseExpression() {

  std::optional<miniplc0::CompilationError> err;
  std::optional<miniplc0::Token> next = nextToken();
  if (!next.has_value()) return {};
  miniplc0::TokenType type = next.value().GetType();
  switch (type) {
    case TokenType::MINUS_SIGN:{
      unreadToken();
      analyseNegateExpression();
      break;
    }
    // 赋值表达式 or 函数调用表达式 or 标识符表达式
    case TokenType::IDENTIFIER:{
      // TODO: 利用符号表判断identifier类型


      // next = nextToken();
      // // TODO
      // if (!next.has_value()) {

      // }
      // type = next.value().GetType();
      // if (type == TokenType::ASSIGN) {
      //   unreadToken();unreadToken();
      //   analyseAssignExpression();
      // }
      // else if (type == TokenType::LEFT_BRACKET) {
      //   unreadToken();unreadToken();
      //   analyseCallExpression();
      // }
      // unreadToken();
      // analyseAssignExpression();
      break;
    }
    case TokenType::UNSIGNED_INTEGER:
    case TokenType::UNSIGNED_DOUBLE: 
    case TokenType::STRING_LITERAL: {
      break;
    }
    // 括号表达式
    case TokenType::LEFT_BRACKET: {
      unreadToken();
      analyseBracketExpression();
      break;
    }
    default:{
      break;
    }
  }

  return {};
}
// <运算符表达式>
std::optional<CompilationError> Analyser::analyseOperatorExpression() {
  return {};
}

// <取反表达式>
// negate_expr -> '-' expr
std::optional<CompilationError> Analyser::analyseNegateExpression() {
  std::optional<miniplc0::CompilationError> err;
  std::optional<miniplc0::Token> next = nextToken();
  if (!next.has_value() || next.value().GetType() != TokenType::MINUS_SIGN) {
    return std::make_optional<CompilationError>(
        _current_pos, ErrorCode::ErrCompiler);
  }

  err = analyseExpression();
  if (err.has_value()) return err;

  return {};
}

// <赋值表达式>
// assign_expr -> l_expr '=' expr
std::optional<CompilationError> Analyser::analyseAssignExpression() {
  
  std::optional<miniplc0::CompilationError> err;
  std::optional<miniplc0::Token> next = nextToken();
  if (!next.has_value() || next.value().GetType() != TokenType::IDENTIFIER) {
    return std::make_optional<CompilationError>(
        _current_pos, ErrorCode::ErrCompiler);
  }

  next = nextToken();
  if (!next.has_value() || next.value().GetType() != TokenType::ASSIGN) {
    return std::make_optional<CompilationError>(
        _current_pos, ErrorCode::ErrCompiler);
  }

  err = analyseExpression();
  if (err.has_value()) return err;

  return {};
}

// <类型转换表达式>
// as_expr -> expr 'as' ty
std::optional<CompilationError> Analyser::analyseAsExpression() {
  std::optional<miniplc0::CompilationError> err;
  std::optional<miniplc0::Token> next = nextToken();
  return {};
}

// <函数调用表达式>
std::optional<CompilationError> Analyser::analyseCallExpression() {
  std::optional<miniplc0::CompilationError> err;
  std::optional<miniplc0::Token> next = nextToken();
  if (!next.has_value() || next.value().GetType() != TokenType::IDENTIFIER) {
    return std::make_optional<CompilationError>(
        _current_pos, ErrorCode::ErrNeedIdentifier);
  }

  next = nextToken();
  if (!next.has_value() || next.value().GetType() != TokenType::LEFT_BRACKET) {
    return std::make_optional<CompilationError>(
        _current_pos, ErrorCode::ErrNeedIdentifier);
  }

  err = analyseCallParameterList();
  if (err.has_value()) return err;

  next = nextToken();
  if (!next.has_value() || next.value().GetType() != TokenType::LEFT_BRACKET) {
    return std::make_optional<CompilationError>(
        _current_pos, ErrorCode::ErrNeedIdentifier);
  }

  return {};
}

// <函数调用参数列表>
std::optional<CompilationError> Analyser::analyseCallParameterList() {
  std::optional<miniplc0::CompilationError> err;
  std::optional<miniplc0::Token> next;

  while (true) {
    err = analyseCallParameterList();
    if (err.has_value()) return err;

    next = nextToken();
    if (!next.has_value() || next.value().GetType() != TokenType::COMMA) {
      unreadToken();
      break;
    }
  }

  return {};
}

// <字面量表达式>
std::optional<CompilationError> Analyser::analyseLiteralExpression() {
  std::optional<miniplc0::CompilationError> err;
  std::optional<miniplc0::Token> next = nextToken();

  if (next.has_value() && next.value().GetType() == TokenType::UNSIGNED_INTEGER) {

  }
  else if (next.has_value() && next.value().GetType() == TokenType::UNSIGNED_DOUBLE) {

  }
  else if (next.has_value() && next.value().GetType() == TokenType::SCIENCE_DOUBLE) {

  }
  else if (next.has_value() && next.value().GetType() == TokenType::STRING_LITERAL) {
    
  } else {
    return std::make_optional<CompilationError>(
        _current_pos, ErrorCode::ErrRecognized);
  }

  return {};
}

// <标识符表达式>
std::optional<CompilationError> Analyser::analyseIdentExpression() {
  std::optional<miniplc0::CompilationError> err;
  std::optional<miniplc0::Token> next = nextToken();
  if (!next.has_value() || next.value().GetType() != TokenType::LEFT_BRACKET) {
    return std::make_optional<CompilationError>(
        _current_pos, ErrorCode::ErrNeedIdentifier);
  }

  return {};
}

// <括号表达式>
std::optional<CompilationError> Analyser::analyseBracketExpression() {
  std::optional<miniplc0::CompilationError> err;
  std::optional<miniplc0::Token> next = nextToken();
  if (!next.has_value() || next.value().GetType() != TokenType::LEFT_BRACKET) {
    return std::make_optional<CompilationError>(
        _current_pos, ErrorCode::ErrNeedBracket);
  }

  err = analyseExpression();
  if (err.has_value()) return err;

  next = nextToken();
  if (!next.has_value() || next.value().GetType() != TokenType::RIGHT_BRACKET) {
    return std::make_optional<CompilationError>(
        _current_pos, ErrorCode::ErrNeedBracket);
  }

  return {};
}

}