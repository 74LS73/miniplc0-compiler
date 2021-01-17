#include "analyser.h"

namespace miniplc0 {

// program -> decl_stmt* function*
ProgNodePtr Analyser::analyseProgram() {
  printf("analyse program\n");
  auto node = std::make_shared<ProgNode>();
  optional<CompilationError> err;

  auto _start = std::make_shared<FuncNode>();
  node->_funcs.emplace_back(_start);
  _start->_name = "_start";
  _start->_return_slots = 0;
  _start->_param_slots = 0;
  _start->_loc_slots = 0;
  _start->_need_ret = false;
  _symbol_table_stack.declareFunction(_start);
  _start->_body = std::make_shared<BlockStatNode>();

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
        _start->_body->_stats.emplace_back(var);
        break;
      }
      default:
        unreadToken();
        throw AnalyserError({_current_pos, ErrorCode::ErrRecognized});
    }
  }

  // call
  auto call_main = std::make_shared<ExprStatNode>();
  auto call_expr = std::make_shared<CallExprNode>();
  call_expr->_name = "main";
  call_expr->_id = _symbol_table_stack.getFunctionByName("main")->_id;
  call_expr->_return_slots = _symbol_table_stack.getFunctionByName("main")->_return_slots;
  call_main->_expr = call_expr;
  _start->_body->_stats.emplace_back(call_main);
  node->_globals = &_symbol_table_stack.getGlobalTable();

  return node;
}
}  // namespace miniplc0