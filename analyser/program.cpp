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

      err = analyseFunction();
      if (err.has_value()) return err;

    } else if (next.value().GetType() == TokenType::LET) {
      unreadToken();

      err = analyseDeclVariableStatement();
      if (err.has_value()) return err;

    } else if (next.value().GetType() == TokenType::CONST) {
      unreadToken();

      err = analyseDeclConstStatement();
      if (err.has_value()) return err;

    } else {
      unreadToken();
      return std::make_optional<CompilationError>(_current_pos,
                                                  ErrorCode::ErrRecognized);
    }
  }
  return {};
}
}  // namespace miniplc0