#include "analyser.h"

namespace miniplc0 {

#define UNREACHABLE()

/* expression -> (unary  operator) | assign_expr

  operator -> operator_expr
  unary    -> negate* item as*
  item     -> call_expr | literal_expr | ident_expr | group_expr
*/

// expr -> (unary  operator) | assign_expr
ExprNodePtr Analyser::analyseExpression() {
  // assign
  auto next = nextToken();
  if (next.has_value() && next.value().GetType() == TokenType::IDENTIFIER) {
    next = nextToken();
    if (next.has_value() && next.value().GetType() == TokenType::ASSIGN) {
      unreadToken();
      unreadToken();
      return analyseAssignExpression();
    }
    unreadToken();
  }
  unreadToken();

  auto unary = analyserUnaryExpression();

  // 是否为符号
  next = nextToken();
  if (!next.has_value() || !next.value().isTokenABinaryOperator()) {
    unreadToken();
    return unary;
  }

  auto op_expr = std::make_shared<OpExprNode>();

  op_expr->_lhs = unary;
  op_expr->_operator = next->GetType();
  unreadToken();
  // shared_ptr<OpExprNode> op_expr;
  // auto lhs = std::dynamic_pointer_cast<OpExprNode>(unary);
  auto op_node = analyseOperatorExpression(op_expr);

  return op_node;
}

// item ->
// | 标识符
// | 函数调用
// | 字面量（INT, DOUBLE, CHAR, STRING）
// | GroupEx
ExprNodePtr Analyser::analyserItemExpression() {
  optional<Token> next = nextToken();
  if (!next.has_value()) {
    throw AnalyserError({_current_pos, ErrorCode::ErrCompiler});
  } else if (next.value().GetType() == TokenType::IDENTIFIER) {
    next = nextToken();

    if (next.has_value() && next.value().GetType() == TokenType::LEFT_BRACKET) {
      // FunctionCall
      unreadToken();
      unreadToken();
      auto res = analyseCallExpression();

    } else {
      // IDentifier
      unreadToken();
      unreadToken();
      auto res = analyseIdentExpression();
    }
  } else if (next.value().GetType() == TokenType::LEFT_BRACKET) {
    unreadToken();
    auto res = analyseBracketExpression();

  } else {
    // 字面量
    unreadToken();
    auto res = analyseLiteralExpression();
  }

  UNREACHABLE();
  return {};
}

// UExpr -> PreUOp* Item ProUOp*
// PreUOp -> '-'
// ProUOp -> 'as' TypeDef
ExprNodePtr Analyser::analyserUnaryExpression() {
  auto node = std::make_shared<UnaryExprNode>();
  optional<Token> next;
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

  auto ident = analyserItemExpression();
  node->_ident = ident;

  if (nega & 1) {
    node->_nega = true;
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
        throw AnalyserError({_current_pos, ErrorCode::ErrNeedType});
      }
      as_flag = true;
    }
  }
  // TODO
  // 显然只有最后一个as的值有用
  if (as_flag) {
    auto as_type = next.value().GetType();
    // lhs->type = as_type;
    // lhs->p_code_gen.generateAs(lhs->type, as_type);
    node->_type = as_type;
  }

  return node;
}

// <运算符表达式> opexpr -> Unary op Unary;
// 5 + 1 + 4 * 3 + 5 * 4 + 2
// 初始token_type记录当前正在分析的运算类型
// 初始为默认值（对应优先级最小）
ExprNodePtr Analyser::analyseOperatorExpression(ExprNodePtr _expr) {
  optional<Token> next;

  auto lhs = std::dynamic_pointer_cast<OpExprNode>(_expr);
  while (true) {
    next = nextToken();
    if (!next.has_value() || !next.value().isTokenABinaryOperator()) {
      unreadToken();
      return lhs;
    }

    auto current_op = next.value();
    if (lhs->_operator != current_op.GetType())
      throw AnalyserError({_current_pos, ErrorCode::ErrCompiler});

    auto rhs = analyserUnaryExpression();

    while (true) {
      next = nextToken();

      if (!next.has_value() || !next.value().isTokenABinaryOperator()) {
        // End
        unreadToken();
        auto node = std::make_shared<OpExprNode>();
        node->_lhs = lhs;
        node->_operator = current_op.GetType();
        node->_rhs = rhs;
        return node;
      } else if (next.value() < current_op) {
        // * 遇上 + ，跳出，结合lhr和rhs
        current_op = next.value();
        unreadToken();
        break;
      }

      // + 遇上 * ，递归，rhs变lhs
      auto new_op_expr = std::make_shared<OpExprNode>();
      new_op_expr->_rhs = rhs;
      new_op_expr->_operator = next->GetType();
      unreadToken();
      rhs = analyseOperatorExpression(new_op_expr);
    }
    // 结合lhs和rhs
    auto tmp = std::make_shared<OpExprNode>();
    tmp->_lhs = lhs;
    tmp->_rhs = rhs;
    tmp->_operator = current_op.GetType();
    lhs = std::make_shared<OpExprNode>();
    lhs->_lhs = tmp;
    lhs->_operator = current_op.GetType();
    // lhs = lhs + rhs;
    // 添加运算符
    // lhs->combine(current_op.GetType(), rhs);
  }

  UNREACHABLE();
  return {};
}

// <赋值表达式>
// assign_expr -> l_expr '=' expr
ExprNodePtr Analyser::analyseAssignExpression() {
  auto node = std::make_shared<AssignExprNode>();
  auto lhs = analyseIdentExpression();
  node->_lhs = lhs;

  auto next = nextToken();
  if (!next.has_value() || next.value().GetType() != TokenType::ASSIGN) {
    throw AnalyserError({_current_pos, ErrorCode::ErrCompiler});
  }

  auto rhs = analyseExpression();
  node->_rhs = rhs;
  return node;
}

// <函数调用表达式>
ExprNodePtr Analyser::analyseCallExpression() {
  auto node = std::make_shared<CallExprNode>();

  optional<miniplc0::Token> next = nextToken();
  if (!next.has_value() || next.value().GetType() != TokenType::IDENTIFIER) {
    throw AnalyserError({_current_pos, ErrorCode::ErrNeedIdentifier});
  }

  auto call_func_name = next.value().GetValueString();
  node->_name = call_func_name;

  next = nextToken();
  if (!next.has_value() || next.value().GetType() != TokenType::LEFT_BRACKET) {
    throw AnalyserError({_current_pos, ErrorCode::ErrNeedBracket});
  }

  next = nextToken();
  if (next.has_value() && next.value().GetType() == TokenType::RIGHT_BRACKET) {
    return {};
  }
  unreadToken();
  // lhs->p_code_gen.generateStackAlloc(call_func.value().return_slots);

  // 传入参数列表
  while (true) {
    auto param = analyseExpression();
    node->_params.emplace_back(param);
    next = nextToken();
    if (!next.has_value() || next.value().GetType() != TokenType::COMMA) {
      unreadToken();
      break;
    }
  }

  next = nextToken();
  if (!next.has_value() || next.value().GetType() != TokenType::RIGHT_BRACKET) {
    throw AnalyserError({_current_pos, ErrorCode::ErrNeedBracket});
  }

  return node;
}

// <字面量表达式>
ExprNodePtr Analyser::analyseLiteralExpression() {
  auto node = std::make_shared<ItemExprNode>();
  optional<miniplc0::Token> next = nextToken();
  if (!next.has_value()) {
    throw AnalyserError({_current_pos, ErrorCode::ErrCompiler});
  } else if (next.value().GetType() == TokenType::UNSIGNED_INTEGER) {
    // 64 位有符号整数 int
    auto val_str = next.value().GetValueString();
    int64_t *val = new int64_t(StringToUnsignedInt64(val_str));

    node->_type = TokenType::INT;
    node->_value = reinterpret_cast<int64_t *>(val);
    // lhs->p_code_gen.generateInt64(val);

  } else if (next.value().GetType() == TokenType::UNSIGNED_DOUBLE) {
    // TODO
    auto val_str = next.value().GetValueString();
    double *val = new double(StringToUnsignedDouble(val_str));

    node->_type = TokenType::DOUBLE;
    node->_value = reinterpret_cast<int64_t *>(val);
    // lhs->p_code_gen.generateDouble(val);

  } else if (next.value().GetType() == TokenType::CHAR_LITERAL) {
  } else if (next.value().GetType() == TokenType::STRING_LITERAL) {
  } else {
    throw AnalyserError({_current_pos, ErrorCode::ErrRecognized});
  }

  return node;
}

// <标识符表达式>
ExprNodePtr Analyser::analyseIdentExpression() {
  auto node = std::make_shared<IdentExprNode>();
  optional<miniplc0::Token> next = nextToken();
  if (!next.has_value() || next.value().GetType() != TokenType::IDENTIFIER) {
    throw AnalyserError({_current_pos, ErrorCode::ErrNeedIdentifier});
  }

  node->_name = next.value().GetValueString();
  // lhs->_type = var.value().type;
  return node;
}

// <括号表达式>
ExprNodePtr Analyser::analyseBracketExpression() {
  optional<miniplc0::Token> next = nextToken();
  if (!next.has_value() || next.value().GetType() != TokenType::LEFT_BRACKET) {
    throw AnalyserError({_current_pos, ErrorCode::ErrNeedBracket});
  }

  auto node = analyseExpression();

  next = nextToken();
  if (!next.has_value() || next.value().GetType() != TokenType::RIGHT_BRACKET) {
    throw AnalyserError({_current_pos, ErrorCode::ErrNeedBracket});
  }

  return node;
}

}  // namespace miniplc0
