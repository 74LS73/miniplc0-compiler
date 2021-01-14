#include "token.h"

namespace miniplc0 {

int32_t getOperatorPriority(const TokenType &t) {
  switch (t) {
    case TokenType::MULT_SIGN:
    case TokenType::DIV_SIGN:
      return 3;
    case TokenType::PLUS_SIGN:
    case TokenType::MINUS_SIGN:
      return 2;
    case TokenType::EQUAL:
    case TokenType::NO_EQUAL:
    case TokenType::LESS_SIGN:
    case TokenType::GREATER_SIGN:
    case TokenType::LESS_EQUAL:
    case TokenType::GREATER_EQUAL:
      return 1;
    default:
      return -1;
  }
}

bool operator<(const Token &a, const TokenType &b) {
  return getOperatorPriority(a.GetType()) < getOperatorPriority(b);
}

bool operator<(const Token &a, const Token &b) {
  return getOperatorPriority(a.GetType()) < getOperatorPriority(b.GetType());
}

std::string Token::Print() {
  std::string name;
  switch (_type) {
    case miniplc0::NULL_TOKEN:
      name = "NullToken";
      break;
    case miniplc0::UNSIGNED_INTEGER:
      name = "UnsignedInteger";
      break;
    case miniplc0::UNSIGNED_DOUBLE:
      name = "UnsignedDouble";
      break;
    // case miniplc0::SCIENCE_DOUBLE:
    //   name = "ScienceDouble";
    //   break;
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
      name = "Comment";
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
      name = "DivSign";
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

  char *res = (char *)malloc(sizeof(char) * 128);
  snprintf(res, 127, "Line: %3lu Column: %3lu Type: %s Value: %s",
           GetStartPos().first, GetStartPos().second, name.c_str(),
           GetValueString().c_str());
  return res;
}

}  // namespace miniplc0
