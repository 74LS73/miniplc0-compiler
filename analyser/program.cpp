#include "analyser.h"

namespace miniplc0 {

// program -> decl_stmt* function*
std::optional<CompilationError> Analyser::analyseProgram() {
  std::optional<miniplc0::CompilationError> err;
  std::optional<miniplc0::Token> next;

  while (true) {
    next = nextToken();
    if (!next.has_value()) {
      break;
    } else if (next.value().GetType() == TokenType::FN) {
      unreadToken();

      auto item = FunctionItem();
      err = analyseFunction(item);
      if (err.has_value()) return err;

      declareFunction(item);
    } else if (next.value().GetType() == TokenType::LET) {
      unreadToken();

      auto item = VariableItem();
      err = analyseDeclVariableStatement(item);
      if (err.has_value()) return err;

      declareVariable(item);
    } else if (next.value().GetType() == TokenType::CONST) {
      unreadToken();

      auto item = VariableItem();
      err = analyseDeclConstStatement(item);
      if (err.has_value()) return err;

      declareVariable(item);
    } else {
      unreadToken();
      return std::make_optional<CompilationError>(_current_pos,
                                                  ErrorCode::ErrRecognized);
    }
  }
  return {};
}
}  // namespace miniplc0