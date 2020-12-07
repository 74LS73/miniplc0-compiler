#include "analyser.h"

namespace miniplc0 {

/* expression -> (unary  operator) | assign_expr

  operator -> operator_expr
  unary    -> negate* item as*
  item     -> call_expr | literal_expr | ident_expr | group_expr
*/

// expr -> (unary  operator) | assign_expr
std::optional<CompilationError> Analyser::analyseExpression() {
  std::optional<miniplc0::CompilationError> err;
  auto next = nextToken();
  if (next.has_value() && next.value().GetType() == TokenType::IDENTIFIER) {
    next = nextToken();
    if (next.has_value() && next.value().GetType() == TokenType::ASSIGN) {
      unreadToken();unreadToken();
      err = analyseAssignExpression();
      if (err.has_value()) return err;
      
      return {};
    }
    unreadToken();
  }
  unreadToken();

  auto lhs = OperatorItem();

  err = analyserUnaryExpression(lhs);
  if (err.has_value()) return err;

  err = analyseOperatorExpression(lhs);
  if (err.has_value()) return err;

  return {};
}

// item ->
// | 标识符
// | 函数调用
// | 字面量（INT, DOUBLE, CHAR, STRING）
// | GroupEx
std::optional<CompilationError> Analyser::analyserItemExpression(
    OperatorItem &lhs) {
  std::optional<miniplc0::CompilationError> err;
  std::optional<miniplc0::Token> next = nextToken();

  if (!next.has_value()) {
    return std::make_optional<CompilationError>(_current_pos,
                                                ErrorCode::ErrCompiler);
  } else if (next.value().GetType() == TokenType::IDENTIFIER) {
    next = nextToken();
    
    if (next.has_value() && next.value().GetType() == TokenType::LEFT_BRACKET) {
      // FunctionCall
      unreadToken();
      unreadToken();
      err = analyseCallExpression();
      if (err.has_value()) return err;

    } else {
      // IDentifier
      unreadToken();unreadToken();
      err = analyseIdentExpression();
      if (err.has_value()) return err;
    }
  } else if (next.value().GetType() == TokenType::UNSIGNED_INTEGER) {
    std::cout << next.value().GetValueString() << std::endl;
    // TODO
  } else if (next.value().GetType() == TokenType::UNSIGNED_DOUBLE) {
    std::cout << next.value().GetValueString() << std::endl;
    // TODO
  } else if (next.value().GetType() == TokenType::CHAR_LITERAL) {
    std::cout << next.value().GetValueString() << std::endl;
    // TODO
  } else if (next.value().GetType() == TokenType::STRING_LITERAL) {
    std::cout << next.value().GetValueString() << std::endl;
    // TODO
  } else if (next.value().GetType() == TokenType::LEFT_BRACKET) {
    unreadToken();
    err = analyseBracketExpression();
    if (err.has_value()) return err;
  } else {
    return std::make_optional<CompilationError>(_current_pos,
                                                ErrorCode::ErrCompiler);
  }

  return {};
}

// UExpr -> PreUOp* Item ProUOp*
// PreUOp -> '-'
// ProUOp -> 'as' TypeDef
std::optional<CompilationError> Analyser::analyserUnaryExpression(
    OperatorItem &lhs) {
  std::optional<miniplc0::CompilationError> err;
  std::optional<miniplc0::Token> next;

  // - *
  while (true) {
    next = nextToken();
    if (!next.has_value() || next.value().GetType() != TokenType::MINUS_SIGN) {
      unreadToken();
      break;
    }
  }

  err = analyserItemExpression(lhs);
  if (err.has_value()) return err;

  while (true) {
    next = nextToken();
    if (!next.has_value() || next.value().GetType() != TokenType::AS) {
      unreadToken();
      break;
    } else {
      next = nextToken();
      if (!next.has_value() || !next.value().isTokenAType()) {
        return std::make_optional<CompilationError>(_current_pos,
                                                    ErrorCode::ErrNeedType);
      }

      // TODO
    }
  }

  return {};
}

// <运算符表达式> opexpr -> Unary op Unary;
// 5 + 1 + 4 * 3 + 5 * 4 + 2
// 初始token_type记录当前正在分析的运算类型
// 初始为默认值（对应优先级最小）
std::optional<CompilationError> Analyser::analyseOperatorExpression(
    OperatorItem &lhs, TokenType token_type) {
  std::optional<miniplc0::CompilationError> err;
  std::optional<miniplc0::Token> next;

  while (true) {
    next = nextToken();
    // 如果之前在计算 * ，且现在遇到 +，则需要停止向下递归
    // 否则继续分析
    if (!next.has_value() || !next.value().isTokenABinaryOperator() ||
        next.value() < token_type) {
      unreadToken();
      return {};
    }

    auto &current_op = next.value();

    auto rhs = OperatorItem();
    err = analyserUnaryExpression(rhs);
    if (err.has_value()) return err;

    while (true) {
      next = nextToken();

      // * 遇上 + ，跳出，结合lhr和rhs
      if (!next.has_value() || !next.value().isTokenABinaryOperator() ||
          next.value() < current_op) {
        unreadToken();
        break;
      }

      current_op = next.value();
      // + 遇上 * ，递归，rhs变lhs
      err = analyseOperatorExpression(rhs, current_op.GetType());
      if (err.has_value()) return err;
    }

    // 结合lhs和rhs
    lhs = lhs + rhs;
    // 添加运算符
    // lhs.body.push_back();
  }

  return {};
}

// <取反表达式>
// negate_expr -> '-' expr
std::optional<CompilationError> Analyser::analyseNegateExpression() {
  std::optional<miniplc0::CompilationError> err;
  std::optional<miniplc0::Token> next = nextToken();
  if (!next.has_value() || next.value().GetType() != TokenType::MINUS_SIGN) {
    return std::make_optional<CompilationError>(_current_pos,
                                                ErrorCode::ErrCompiler);
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
    return std::make_optional<CompilationError>(_current_pos,
                                                ErrorCode::ErrCompiler);
  }

  next = nextToken();
  if (!next.has_value() || next.value().GetType() != TokenType::ASSIGN) {
    return std::make_optional<CompilationError>(_current_pos,
                                                ErrorCode::ErrCompiler);
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
    return std::make_optional<CompilationError>(_current_pos,
                                                ErrorCode::ErrNeedIdentifier);
  }

  next = nextToken();
  if (!next.has_value() || next.value().GetType() != TokenType::LEFT_BRACKET) {
    return std::make_optional<CompilationError>(_current_pos,
                                                ErrorCode::ErrNeedBracket);
  }

  // 传入参数列表
  next = nextToken();
  if (next.has_value() && next.value().GetType() == TokenType::RIGHT_BRACKET) {
    goto FINISH;
  }
  unreadToken();
  
  while (true) {
    err = analyseExpression();
    if (err.has_value()) return err;
    next = nextToken();
    if (!next.has_value() || next.value().GetType() != TokenType::COMMA) {
      unreadToken();
      break;
    }
  }
  
  if (!next.has_value() || next.value().GetType() != TokenType::RIGHT_BRACKET) {
    return std::make_optional<CompilationError>(_current_pos,
                                                ErrorCode::ErrNeedBracket);
  }

  FINISH:
  return {};
}

// <字面量表达式>
std::optional<CompilationError> Analyser::analyseLiteralExpression() {
  std::optional<miniplc0::CompilationError> err;
  std::optional<miniplc0::Token> next = nextToken();

  if (next.has_value() &&
      next.value().GetType() == TokenType::UNSIGNED_INTEGER) {
  } else if (next.has_value() &&
             next.value().GetType() == TokenType::UNSIGNED_DOUBLE) {
  } else if (next.has_value() &&
             next.value().GetType() == TokenType::SCIENCE_DOUBLE) {
  } else if (next.has_value() &&
             next.value().GetType() == TokenType::STRING_LITERAL) {
  } else {
    return std::make_optional<CompilationError>(_current_pos,
                                                ErrorCode::ErrRecognized);
  }

  return {};
}

// <标识符表达式>
std::optional<CompilationError> Analyser::analyseIdentExpression() {
  std::optional<miniplc0::CompilationError> err;
  std::optional<miniplc0::Token> next = nextToken();
  if (!next.has_value() || next.value().GetType() != TokenType::IDENTIFIER) {
    return std::make_optional<CompilationError>(_current_pos,
                                                ErrorCode::ErrNeedIdentifier);
  }

  return {};
}

// <括号表达式>
std::optional<CompilationError> Analyser::analyseBracketExpression() {
  std::optional<miniplc0::CompilationError> err;
  std::optional<miniplc0::Token> next = nextToken();
  if (!next.has_value() || next.value().GetType() != TokenType::LEFT_BRACKET) {
    return std::make_optional<CompilationError>(_current_pos,
                                                ErrorCode::ErrNeedBracket);
  }

  err = analyseExpression();
  if (err.has_value()) return err;

  next = nextToken();
  if (!next.has_value() || next.value().GetType() != TokenType::RIGHT_BRACKET) {
    return std::make_optional<CompilationError>(_current_pos,
                                                ErrorCode::ErrNeedBracket);
  }

  return {};
}

}  // namespace miniplc0