#include "analyser.h"

namespace miniplc0 {

// program -> decl_stmt* function*
ProgNodePtr Analyser::analyseProgram() {
  auto node = std::make_shared<ProgNode>();
  optional<CompilationError> err;

  auto _start = std::make_shared<FuncNode>();
  _start->_name = "_start";

  while (true) {
    auto next = nextToken();
    if (!next.has_value()) break;
    switch (next.value().GetType()) {
      case TokenType::FN: {
        unreadToken();
        auto func = analyseFunction();
        node->_funcs.emplace_back(func);
        break;
      }
      case TokenType::LET:
      case TokenType::CONST: {
        unreadToken();
        auto var = analyseDeclStatement(VariableType::GLOBAL);
        node->_vars.emplace_back(var);
        break;
      }
      default:
        unreadToken();
        throw AnalyserError({_current_pos, ErrorCode::ErrRecognized});
    }
  }

  // call
  auto call_main = std::make_shared<ExprStatNode>();
  auto call_expr =  std::make_shared<CallExprNode>();
  call_expr->_name = "main";
  call_main->_expr =call_expr;
  // node->_funcs.emplace_back(_start);
  return node;
}
}  // namespace miniplc0