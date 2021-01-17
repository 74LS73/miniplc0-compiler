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
  op_expr->_type = unary->_type;

  unreadToken();
  // shared_ptr<OpExprNode> op_expr;
  // auto lhs = std::dynamic_pointer_cast<OpExprNode>(unary);

  return analyseOperatorExpression(op_expr);
}

// item ->
// | 标识符
// | 函数调用
// | 字面量（INT, DOUBLE, CHAR, STRING）
// | GroupEx
ExprNodePtr Analyser::analyserItemExpression() {
  optional<Token> next = nextToken();
  if (!next.has_value()) {
    throw ErrorCode::ErrCompiler;
  } else if (next.value().GetType() == TokenType::IDENTIFIER) {
    next = nextToken();

    if (next.has_value() && next.value().GetType() == TokenType::LEFT_BRACKET) {
      // FunctionCall
      unreadToken();
      unreadToken();
      return analyseCallExpression();

    } else {
      // IDentifier
      unreadToken();
      unreadToken();
      return analyseIdentExpression();
    }
  } else if (next.value().GetType() == TokenType::LEFT_BRACKET) {
    unreadToken();
    return analyseBracketExpression();

  } else {
    // 字面量
    unreadToken();
    return analyseLiteralExpression();
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

  auto item = analyserItemExpression();
  node->_item = item;
  if (nega & 1) {
    node->_nega = true;
  }

  // AS
  int as_flag = false;
  TokenType as_type;
  while (true) {
    next = nextToken();
    if (!next.has_value() || next.value().GetType() != TokenType::AS) {
      unreadToken();
      break;
    } else {
      next = nextToken();
      if (!next.has_value() || !next.value().isTokenAType()) {
        throw ErrorCode::ErrNeedType;
      }
      as_type = next->GetType();
      as_flag = true;
    }
  }
  // 显然只有最后一个as的值有用
  if (as_flag) {
    node->_type = as_type;
  } else {
    node->_type = item->_type;
  }
  return node;
}

// <运算符表达式> opexpr -> Unary op Unary;
// 5 + 1 + 4 * 3 + 5 * 4 + 2
// 初始token_type记录当前正在分析的运算类型
// 初始为默认值（对应优先级最小）
ExprNodePtr Analyser::analyseOperatorExpression(OpExprNodePtr _expr) {
  optional<Token> next;

  auto lhs = _expr;
  while (true) {
    next = nextToken();
    if (!next.has_value() || !next.value().isTokenABinaryOperator()) {
      unreadToken();
      return lhs;
    }
    auto current_op = next.value();
    // if (lhs->_operator != current_op.GetType()) {
    //   std::cout << lhs->_operator << std::endl;
    //   std::cout << current_op.GetType() << std::endl;
    //   throw ErrorCode::ErrCompiler;
    // }

    auto rhs = analyserUnaryExpression();

    while (true) {
      next = nextToken();

      if (!next.has_value() || !next.value().isTokenABinaryOperator()) {
        // End
        unreadToken();
        auto node = std::make_shared<OpExprNode>();
        if (lhs->_type != rhs->_type) {
          throw ErrorCode::ErrInvalidAssignment;
        }
        node->_type = lhs->_type;
        node->_lhs = lhs;
        node->_operator = current_op.GetType();
        node->_rhs = rhs;
        return node;
      } else if (next.value() < current_op) {
        // * 遇上 + ，跳出，结合lhr和rhs
        unreadToken();
        break;
      } else {
        // + 遇上 * ，递归，rhs变lhs
        auto new_op_expr = std::make_shared<OpExprNode>();
        new_op_expr->_lhs = rhs;
        new_op_expr->_type = rhs->_type;
        new_op_expr->_operator = next->GetType();
        unreadToken();
        rhs = analyseOperatorExpression(new_op_expr);
      }
    }
    // 结合lhs和rhs
    auto tmp = std::make_shared<OpExprNode>();
    if (lhs->_type != rhs->_type) {
      throw ErrorCode::ErrInvalidAssignment;
    }
    tmp->_type = lhs->_type;
    tmp->_lhs = lhs;
    tmp->_rhs = rhs;
    tmp->_operator = current_op.GetType();
    lhs = std::make_shared<OpExprNode>();
    lhs->_lhs = tmp;
    lhs->_operator = current_op.GetType();
    lhs->_type = tmp->_type;
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

  auto next = nextToken();
  if (!next.has_value() || next.value().GetType() != TokenType::ASSIGN) {
    throw ErrorCode::ErrCompiler;
  }
  if (lhs->_const) {
    throw ErrorCode::ErrAssignToConstant;
  }

  auto rhs = analyseExpression();
  if (lhs->_type != rhs->_type) {
    throw ErrorCode::ErrInvalidAssignment;
  }

  node->_lhs = lhs;
  node->_rhs = rhs;
  return node;
}

// <函数调用表达式>
ExprNodePtr Analyser::analyseCallExpression() {
  auto node = std::make_shared<CallExprNode>();

  auto next = nextToken();
  if (!next.has_value() || next.value().GetType() != TokenType::IDENTIFIER) {
    throw ErrorCode::ErrNeedIdentifier;
  }

  auto call_func_name = next.value().GetValueString();
  auto func = _symbol_table_stack.getFunctionByName(call_func_name);
  node->_name = call_func_name;
  node->_id = func->_id;
  node->_return_slots = func->_return_slots;
  node->_type = func->_return_type;
  node->_is_std = func->_is_std;
  node->_func = func;

  next = nextToken();
  if (!next.has_value() || next.value().GetType() != TokenType::LEFT_BRACKET) {
    throw ErrorCode::ErrNeedBracket;
  }

  // 传入参数列表
  int i = 0;
  for (auto &fun_param : func->_params) {
    auto param = analyseExpression();
    if (param->_type != fun_param->_type) {
      // TODO
      throw ErrorCode::ErrInvalidAssignment;
    }
    node->_params.emplace_back(param);
    i++;
    if (i == func->_params.size()) break;
    next = nextToken();
    if (!next.has_value() || next.value().GetType() != TokenType::COMMA) {
      throw ErrorCode::ErrInvalidAssignment;
    }
  }

  next = nextToken();
  if (!next.has_value() || next.value().GetType() != TokenType::RIGHT_BRACKET) {
    throw ErrorCode::ErrNeedBracket;
  }

  return node;
}

// <字面量表达式>
ExprNodePtr Analyser::analyseLiteralExpression() {
  auto node = std::make_shared<ItemExprNode>();
  optional<miniplc0::Token> next = nextToken();
  if (!next.has_value()) {
    throw ErrorCode::ErrCompiler;
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
    throw ErrorCode::ErrRecognized;
  }

  return node;
}

// <标识符表达式>
IdentExprNodePtr Analyser::analyseIdentExpression() {
  auto node = std::make_shared<IdentExprNode>();
  optional<miniplc0::Token> next = nextToken();
  if (!next.has_value() || next.value().GetType() != TokenType::IDENTIFIER) {
    throw ErrorCode::ErrNeedIdentifier;
  }
  node->_name = next.value().GetValueString();
  auto var = _symbol_table_stack.getVariableByName(node->_name);

  if (var->_const) {
    throw ErrorCode::ErrAssignToConstant;
  }
  // lhs->_type = var.value().type;
  node->_type = var->_type;
  node->_id = var->_id;
  node->_vscope = var->_vscope;
  return node;
}

// <括号表达式>
ExprNodePtr Analyser::analyseBracketExpression() {
  optional<miniplc0::Token> next = nextToken();
  if (!next.has_value() || next.value().GetType() != TokenType::LEFT_BRACKET) {
    throw ErrorCode::ErrNeedBracket;
  }

  auto node = analyseExpression();

  next = nextToken();
  if (!next.has_value() || next.value().GetType() != TokenType::RIGHT_BRACKET) {
    throw ErrorCode::ErrNeedBracket;
  }

  return node;
}

}  // namespace miniplc0
