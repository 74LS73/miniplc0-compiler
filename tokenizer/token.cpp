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

bool operator< (const Token &a, const TokenType &b) {
  return getOperatorPriority(a.GetType()) < getOperatorPriority(b);
}

bool operator<  (const Token &a, const Token &b) {
  return getOperatorPriority(a.GetType()) < getOperatorPriority(b.GetType());
}

}  // namespace miniplc0