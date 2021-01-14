#include "error.h"

namespace miniplc0 {

std::string CompilationError::GetString() {
  std::string name;
  switch (_err) {
    case miniplc0::ErrNoError:
      name = "No error.";
      break;
    case miniplc0::ErrStreamError:
      name = "Stream error.";
      break;
    case miniplc0::ErrEOF:
      name = "EOF";
      break;
    case miniplc0::ErrCompiler:
      name = "编译错误。";
      break;
    case miniplc0::ErrInvalidInput:
      name = "The input is invalid.";
      break;
    case miniplc0::ErrInvalidIdentifier:
      name = "Identifier is invalid";
      break;
    case miniplc0::ErrIntegerOverflow:
      name = "The integer is too big(int64_t).";
      break;
    case miniplc0::ErrNeedIdentifier:
      name = "Need an identifier here.";
      break;
    case miniplc0::ErrNeedSemicolon:
      name = "Need a semicolon here.";
      break;
    case miniplc0::ErrNeedColon:
      name = "Need a colon here.";
      break;
    case miniplc0::ErrNeedDeclareSymbol:
      name = "Need a declare symbol here.";
      break;
    case miniplc0::ErrNeedReturn:
      name = "Need a return here.";
      break;
    case miniplc0::ErrNeedBracket:
      name = "Need a bracket here.";
      break;
    case miniplc0::ErrNeedBrace:
      name = "Need a brace here.";
      break;
    case miniplc0::ErrNeedType:
      name = "Need a type here.";
      break;
    case miniplc0::ErrNeedArrow:
      name = "Need an arrow here.";
      break;
    case miniplc0::ErrNeedOperator:
      name = "Need an operator here.";
      break;
    case miniplc0::ErrConstantNeedValue:
      name = "Const variable need an value here.";
      break;
    case miniplc0::ErrInvalidVariableDeclaration:
      name = "The declaration is invalid.";
      break;
    case miniplc0::ErrIncompleteExpression:
      name = "The expression is incomplete.";
      break;
    case miniplc0::ErrNotDeclared:
      name = "The variable or constant must be declared before being used.";
      break;
    case miniplc0::ErrAssignToConstant:
      name = "Trying to assign value to a constant.";
      break;
    case miniplc0::ErrDuplicateDeclaration:
      name = "The variable or constant has been declared.";
      break;
    case miniplc0::ErrNotInitialized:
      name = "The variable has not been initialized.";
      break;
    case miniplc0::ErrInvalidAssignment:
      name = "The assignment statement is invalid.";
      break;
    case miniplc0::ErrInvalidPrint:
      name = "The output statement is invalid.";
      break;
    case miniplc0::ErrRecognized:
      name = "The token is recognized.";
      break;
  }
  return name;
}

}  // namespace miniplc0
