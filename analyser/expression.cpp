#include "analyser.h"

namespace miniplc0 {

/* expression -> (unary  operator) | assign_expr

  operator -> operator_expr
  unary    -> negate* item as*
  item     -> call_expr | literal_expr | ident_expr | group_expr
*/

// expr -> (unary  operator) | assign_expr
std::optional<CompilationError> Analyser::analyseExpression(
    std::shared_ptr<Item> lhs) {
  std::optional<miniplc0::CompilationError> err;

  // assign
  auto next = nextToken();
  if (next.has_value() && next.value().GetType() == TokenType::IDENTIFIER) {
    next = nextToken();
    if (next.has_value() && next.value().GetType() == TokenType::ASSIGN) {
      unreadToken();
      unreadToken();
      err = analyseAssignExpression(lhs);
      if (err.has_value()) return err;

      return {};
    }
    unreadToken();
  }
  unreadToken();

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
    std::shared_ptr<Item> lhs) {
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
      err = analyseCallExpression(lhs);
      if (err.has_value()) return err;

    } else {
      // IDentifier
      unreadToken();
      unreadToken();
      err = analyseIdentExpression(lhs);
      if (err.has_value()) return err;
    }
  } else if (next.value().GetType() == TokenType::LEFT_BRACKET) {
    unreadToken();
    err = analyseBracketExpression(lhs);
    if (err.has_value()) return err;
  } else {
    // 字面量
    unreadToken();
    err = analyseLiteralExpression(lhs);
    if (err.has_value()) return err;
  }

  return {};
}

// UExpr -> PreUOp* Item ProUOp*
// PreUOp -> '-'
// ProUOp -> 'as' TypeDef
std::optional<CompilationError> Analyser::analyserUnaryExpression(
    std::shared_ptr<Item> lhs) {
  std::optional<miniplc0::CompilationError> err;
  std::optional<miniplc0::Token> next;

  // - *
  int64_t nega = 0;
  while (true) {
    next = nextToken();
    if (!next.has_value() || next.value().GetType() != TokenType::MINUS_SIGN) {
      unreadToken();
      break;
    }
    ++nega;
  }

  err = analyserItemExpression(lhs);
  if (err.has_value()) return err;

  if (nega & 1) {
    lhs->p_code_gen.generateNega(lhs->type);
  }

  // AS
  int as_flag = false;
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
      as_flag = true;
    }
  }
  // TODO
  // 显然只有最后一个as的值有用
  if (as_flag) {
    auto as_type = next.value().GetType();
    lhs->type = as_type;
    lhs->p_code_gen.generateAs(lhs->type, as_type);
  }
  return {};
}

// <运算符表达式> opexpr -> Unary op Unary;
// 5 + 1 + 4 * 3 + 5 * 4 + 2
// 初始token_type记录当前正在分析的运算类型
// 初始为默认值（对应优先级最小）
std::optional<CompilationError> Analyser::analyseOperatorExpression(
    std::shared_ptr<Item> lhs, TokenType token_type) {
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

    auto current_op = next.value();

    auto rhs = std::shared_ptr<Item>(new Item());
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

      auto next_op = next.value();
      // + 遇上 * ，递归，rhs变lhs
      unreadToken();
      err = analyseOperatorExpression(rhs, next_op.GetType());
      if (err.has_value()) return err;
    }

    // 结合lhs和rhs
    // lhs = lhs + rhs;
    // 添加运算符
    lhs->combine(current_op.GetType(), rhs);
  }

  return {};
}

// <赋值表达式>
// assign_expr -> l_expr '=' expr
std::optional<CompilationError> Analyser::analyseAssignExpression(
    std::shared_ptr<Item> lhs) {
  std::optional<miniplc0::CompilationError> err;
  std::optional<miniplc0::Token> next = nextToken();
  if (!next.has_value() || next.value().GetType() != TokenType::IDENTIFIER) {
    return std::make_optional<CompilationError>(_current_pos,
                                                ErrorCode::ErrCompiler);
  }

  auto var_name = next.value().GetValueString();
  auto var = _symbol_table_stack.getVariableByName(var_name);
  if (!var.has_value()) {
    return std::make_optional<CompilationError>(
        _current_pos, ErrorCode::ErrNeedDeclareSymbol);
  }
  if (var.value().is_const) {
    return std::make_optional<CompilationError>(_current_pos,
                                                ErrorCode::ErrAssignToConstant);
  }

  lhs->p_code_gen.generateGetVariable(var->id, var->vt);

  next = nextToken();
  if (!next.has_value() || next.value().GetType() != TokenType::ASSIGN) {
    return std::make_optional<CompilationError>(_current_pos,
                                                ErrorCode::ErrCompiler);
  }

  err = analyseExpression(lhs);
  if (err.has_value()) return err;

  lhs->p_code_gen.generateStore();
  return {};
}

// // <类型转换表达式>
// // as_expr -> expr 'as' ty
// std::optional<CompilationError> Analyser::analyseAsExpression() {
//   std::optional<miniplc0::CompilationError> err;
//   std::optional<miniplc0::Token> next = nextToken();
//   return {};
// }

// <函数调用表达式>
std::optional<CompilationError> Analyser::analyseCallExpression(
    std::shared_ptr<Item> lhs) {
  std::optional<miniplc0::CompilationError> err;

  std::optional<miniplc0::Token> next = nextToken();

  if (!next.has_value() || next.value().GetType() != TokenType::IDENTIFIER) {
    return std::make_optional<CompilationError>(_current_pos,
                                                ErrorCode::ErrNeedIdentifier);
  }

  auto call_func_name = next.value().GetValueString();
  auto call_func = _symbol_table_stack.getFunctionByName(call_func_name);

  next = nextToken();
  if (!next.has_value() || next.value().GetType() != TokenType::LEFT_BRACKET) {
    return std::make_optional<CompilationError>(_current_pos,
                                                ErrorCode::ErrNeedBracket);
  }

  lhs->p_code_gen.generateStackAlloc(call_func.value().return_slots);

  // 传入参数列表
  // TODO
  for (int i = 0; i < call_func.value().param_slots; ++i) {
    auto param = call_func.value().params[i];
    err = analyseExpression(lhs);
    if (err.has_value())
      return std::make_optional<CompilationError>(_current_pos,
                                                  ErrorCode::ErrNeedIdentifier);

    if (i == call_func.value().param_slots - 1) break;

    next = nextToken();
    if (!next.has_value() || next.value().GetType() != TokenType::COMMA) {
      return std::make_optional<CompilationError>(_current_pos,
                                                  ErrorCode::ErrNeedIdentifier);
    }
  }

  next = nextToken();
  if (!next.has_value() || next.value().GetType() != TokenType::RIGHT_BRACKET) {
    return std::make_optional<CompilationError>(_current_pos,
                                                ErrorCode::ErrNeedBracket);
  }

  //  StackAlloc(0)
  //    26: Push(2)
  //    27: Push(22)
  //    28: Call(1)
  lhs->p_code_gen.generateCallFunction(call_func->id);

  return {};
}

// <字面量表达式>
std::optional<CompilationError> Analyser::analyseLiteralExpression(
    std::shared_ptr<Item> lhs) {
  std::optional<miniplc0::CompilationError> err;
  std::optional<miniplc0::Token> next = nextToken();

  if (next.has_value() &&
      next.value().GetType() == TokenType::UNSIGNED_INTEGER) {
    // 64 位有符号整数 int
    auto val_str = next.value().GetValueString();
    int64_t val = StringToUnsignedInt64(val_str);

    lhs->type = TokenType::INT;
    lhs->p_code_gen.generateInt64(val);

  } else if (next.has_value() &&
             next.value().GetType() == TokenType::UNSIGNED_DOUBLE) {
    // TODO
    auto val_str = next.value().GetValueString();
    double val = StringToUnsignedDouble(val_str);

    lhs->type = TokenType::DOUBLE;
    lhs->p_code_gen.generateDouble(val);

  } else if (next.has_value() &&
             next.value().GetType() == TokenType::CHAR_LITERAL) {
  } else if (next.has_value() &&
             next.value().GetType() == TokenType::STRING_LITERAL) {
  } else {
    return std::make_optional<CompilationError>(_current_pos,
                                                ErrorCode::ErrRecognized);
  }

  return {};
}

// <标识符表达式>
std::optional<CompilationError> Analyser::analyseIdentExpression(
    std::shared_ptr<Item> lhs) {
  std::optional<miniplc0::CompilationError> err;
  std::optional<miniplc0::Token> next = nextToken();
  if (!next.has_value() || next.value().GetType() != TokenType::IDENTIFIER) {
    return std::make_optional<CompilationError>(_current_pos,
                                                ErrorCode::ErrNeedIdentifier);
  }

  auto ident_name = next.value().GetValueString();
  auto var = _symbol_table_stack.getVariableByName(ident_name);
  if (!var.has_value()) {
    return std::make_optional<CompilationError>(
        _current_pos, ErrorCode::ErrNeedDeclareSymbol);
  }
  switch (var.value().vt) {
    case VariableType::LOCAL:
    case VariableType::GLOBAL:
    case VariableType::PARAM:
      lhs->p_code_gen.generateLoadVariable(var.value().id, var.value().vt);
      break;
    default:
      return std::make_optional<CompilationError>(_current_pos,
                                                  ErrorCode::ErrCompiler);
  }

  lhs->type = var.value().type;
  return {};
}

// <括号表达式>
std::optional<CompilationError> Analyser::analyseBracketExpression(
    std::shared_ptr<Item> lhs) {
  std::optional<miniplc0::CompilationError> err;
  std::optional<miniplc0::Token> next = nextToken();
  if (!next.has_value() || next.value().GetType() != TokenType::LEFT_BRACKET) {
    return std::make_optional<CompilationError>(_current_pos,
                                                ErrorCode::ErrNeedBracket);
  }

  err = analyseExpression(lhs);
  if (err.has_value()) return err;

  next = nextToken();
  if (!next.has_value() || next.value().GetType() != TokenType::RIGHT_BRACKET) {
    return std::make_optional<CompilationError>(_current_pos,
                                                ErrorCode::ErrNeedBracket);
  }

  return {};
}

}  // namespace miniplc0
