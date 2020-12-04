#include "analyser/analyser.h"
#include "fmt/core.h"
#include "tokenizer/tokenizer.h"

namespace fmt {
template <>
struct formatter<miniplc0::ErrorCode> {
  template <typename ParseContext>
  constexpr auto parse(ParseContext &ctx) {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(const miniplc0::ErrorCode &p, FormatContext &ctx) {
    std::string name;
    switch (p) {
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
    return format_to(ctx.out(), name);
  }
};

template <>
struct formatter<miniplc0::CompilationError> {
  template <typename ParseContext>
  constexpr auto parse(ParseContext &ctx) {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(const miniplc0::CompilationError &p, FormatContext &ctx) {
    return format_to(ctx.out(), "Line: {} Column: {} Error: {}",
                     p.GetPos().first, p.GetPos().second, p.GetCode());
  }
};
}  // namespace fmt

namespace fmt {
template <>
struct formatter<miniplc0::Token> {
  template <typename ParseContext>
  constexpr auto parse(ParseContext &ctx) {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(const miniplc0::Token &p, FormatContext &ctx) {
    return format_to(ctx.out(), "Line: {} Column: {} Type: {} Value: {}",
                     p.GetStartPos().first, p.GetStartPos().second, p.GetType(),
                     p.GetValueString());
  }
};

template <>
struct formatter<miniplc0::TokenType> {
  template <typename ParseContext>
  constexpr auto parse(ParseContext &ctx) {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(const miniplc0::TokenType &p, FormatContext &ctx) {
    std::string name;
    switch (p) {
      case miniplc0::NULL_TOKEN:
        name = "NullToken";
        break;
      case miniplc0::UNSIGNED_INTEGER:
        name = "UnsignedInteger";
        break;
      case miniplc0::UNSIGNED_DOUBLE:
        name = "UnsignedDouble";
        break;
      case miniplc0::SCIENCE_DOUBLE:
        name = "ScienceInteger";
        break;
      case miniplc0::CHAR_LITERAL:
        name = "CharLiteral";
        break;
      case miniplc0::IDENTIFIER:
        name = "Identifier";
        break;
      case miniplc0::STRING_LITERAL:
        name = "StringLiteral";
        break;
      case miniplc0::COMMENT:
        name = "StringLiteral";
        break;

      case miniplc0::INT:
        name = "Int";
        break;
      case miniplc0::VOID:
        name = "Void";
        break;
      case miniplc0::DOUBLE:
        name = "Double";
        break;
      case miniplc0::FN:
        name = "Fn";
        break;
      case miniplc0::LET:
        name = "Let";
        break;
      case miniplc0::CONST:
        name = "Const";
        break;
      case miniplc0::AS:
        name = "As";
        break;
      case miniplc0::WHILE:
        name = "While";
        break;
      case miniplc0::IF:
        name = "If";
        break;
      case miniplc0::ELSE:
        name = "Else";
        break;
      case miniplc0::BREAK:
        name = "Break";
        break;
      case miniplc0::CONTINUE:
        name = "Continue";
        break;
      case miniplc0::RETURN:
        name = "Return";
        break;

      case miniplc0::PLUS_SIGN:
        name = "PlusSign";
        break;
      case miniplc0::MINUS_SIGN:
        name = "MinusSign";
        break;
      case miniplc0::MULT_SIGN:
        name = "Semicolon";
        break;
      case miniplc0::DIV_SIGN:
        name = "LeftBracket";
        break;
      case miniplc0::ASSIGN:
        name = "RightBracket";
        break;
      case miniplc0::EQUAL:
        name = "RightBracket";
        break;

      case miniplc0::NO_EQUAL:
        name = "NoEqual";
        break;
      case miniplc0::LESS_SIGN:  // <
        name = "LessSign";
        break;
      case miniplc0::GREATER_SIGN:  // >
        name = "GreaterSign";
        break;
      case miniplc0::LESS_EQUAL:  // <=
        name = "LessEqual";
        break;
      case miniplc0::GREATER_EQUAL:  // >=
        name = "GreaterEqual";
        break;
      case miniplc0::LEFT_BRACKET:  // (
        name = "LeftBracket";
        break;
      case miniplc0::RIGHT_BRACKET:  // )
        name = "RightBracket";
        break;
      case miniplc0::LEFT_BRACE:  // {
        name = "LeftBrace";
        break;
      case miniplc0::RIGHT_BRACE:  // }
        name = "RightBrace";
        break;
      case miniplc0::ARROW:  // ->
        name = "Arrow";
        break;
      case miniplc0::COMMA:  // ,
        name = "Comma";
        break;
      case miniplc0::COLON:  // :
        name = "Colon";
        break;
      case miniplc0::SEMICOLON:  // ;
        name = "Semicolon";
        break;
    }
    return format_to(ctx.out(), name);
  }
};
}  // namespace fmt

namespace fmt {
template <>
struct formatter<miniplc0::Operation> {
  template <typename ParseContext>
  constexpr auto parse(ParseContext &ctx) {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(const miniplc0::Operation &p, FormatContext &ctx) {
    std::string name;
    switch (p) {
      case miniplc0::ILL:
        name = "ILL";
        break;
      case miniplc0::ADD:
        name = "ADD";
        break;
      case miniplc0::SUB:
        name = "SUB";
        break;
      case miniplc0::MUL:
        name = "MUL";
        break;
      case miniplc0::DIV:
        name = "DIV";
        break;
      case miniplc0::WRT:
        name = "WRT";
        break;
      case miniplc0::LIT:
        name = "LIT";
        break;
      case miniplc0::LOD:
        name = "LOD";
        break;
      case miniplc0::STO:
        name = "STO";
        break;
    }
    return format_to(ctx.out(), name);
  }
};
template <>
struct formatter<miniplc0::Instruction> {
  template <typename ParseContext>
  constexpr auto parse(ParseContext &ctx) {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(const miniplc0::Instruction &p, FormatContext &ctx) {
    std::string name;
    switch (p.GetOperation()) {
      case miniplc0::ILL:
      case miniplc0::ADD:
      case miniplc0::SUB:
      case miniplc0::MUL:
      case miniplc0::DIV:
      case miniplc0::WRT:
        return format_to(ctx.out(), "{}", p.GetOperation());
      case miniplc0::LIT:
      case miniplc0::LOD:
      case miniplc0::STO:
        return format_to(ctx.out(), "{} {}", p.GetOperation(), p.GetX());
    }
    return format_to(ctx.out(), "ILL");
  }
};
}  // namespace fmt
