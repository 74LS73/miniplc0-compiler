#include "analyser.h"

namespace miniplc0 {

// program -> decl_stmt* function*
optional<CompilationError> Analyser::analyseProgram() {
  auto node = std::make_shared<ProgNode>();
  optional<CompilationError> err;

  auto _start = std::make_shared<FuncNode>();
  auto ident = std::make_shared<IdentExprNode>();
  ident->_name = "_start";
  _start->_ident = ident;

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
        auto var = analyseDeclStatement();
        node->_vars.emplace_back(var);
        break;
      }
      default:
        unreadToken();
        throw AnalyserError({_current_pos, ErrorCode::ErrRecognized});
    }
  }

  // call
  // auto call_main = std::make_shared<ExprStatNode>();
  // auto call_expr =  std::make_shared<CallExprNode>();
  // call_expr->_name = "main";
  // call_main->_expr =call_expr;
  // _start->_body->_stats.emplace_back(call_main);
  // node->_funcs.emplace_back(_start);
  return {};
}
}  // namespace miniplc0