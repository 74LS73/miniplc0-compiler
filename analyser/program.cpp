#include "analyser.h"

namespace miniplc0 {

// program -> decl_stmt* function*
std::optional<CompilationError> Analyser::analyseProgram() {
  std::optional<miniplc0::CompilationError> err;
  std::optional<miniplc0::Token> next;

  auto _start = FunctionItem();
  _start.name = "_start";
  auto _start_token = Token(TokenType::IDENTIFIER, _start.name, 0, 0, 0, 0);

  while (true) {
    next = nextToken();
    if (!next.has_value()) {
      break;
    } else if (next.value().GetType() == TokenType::FN) {
      unreadToken();

      err = analyseFunction();
      if (err.has_value()) return err;

    } else if (next.value().GetType() == TokenType::LET) {
      unreadToken();

      err = analyseDeclVariableStatement(_start, VariableType::GLOBAL);
      if (err.has_value()) return err;

    } else if (next.value().GetType() == TokenType::CONST) {
      unreadToken();

      err = analyseDeclConstStatement(_start, VariableType::GLOBAL);
      if (err.has_value()) return err;

    } else {
      unreadToken();
      return std::make_optional<CompilationError>(_current_pos,
                                                  ErrorCode::ErrRecognized);
    }
  }

  _symbol_table_stack.declareFunction(_start_token, _start);

  // call main();
  return {};
}
}  // namespace miniplc0