#include "tokenizer/tokenizer.h"

#include <cctype>
#include <sstream>

namespace miniplc0 {

std::pair<std::optional<Token>, std::optional<CompilationError>>
Tokenizer::NextToken() {
  if (!_initialized) readAll();
  if (_rdr.bad())
    return std::make_pair(
        std::optional<Token>(),
        std::make_optional<CompilationError>(0, 0, ErrorCode::ErrStreamError));
  if (isEOF())
    return std::make_pair(
        std::optional<Token>(),
        std::make_optional<CompilationError>(0, 0, ErrorCode::ErrEOF));
  auto p = nextToken();
  if (p.second.has_value()) return std::make_pair(p.first, p.second);
  auto err = checkToken(p.first.value());
  if (err.has_value()) return std::make_pair(p.first, err.value());
  return std::make_pair(p.first, std::optional<CompilationError>());
}

std::pair<std::vector<Token>, std::optional<CompilationError>>
Tokenizer::AllTokens() {
  std::vector<Token> result;
  while (true) {
    auto p = NextToken();
    if (p.second.has_value()) {
      if (p.second.value().GetCode() == ErrorCode::ErrEOF)
        return std::make_pair(result, std::optional<CompilationError>());
      else
        return std::make_pair(std::vector<Token>(), p.second);
    }
    result.emplace_back(p.first.value());
  }
}

// 注意：这里的返回值中 Token 和 CompilationError 只能返回一个，不能同时返回。
std::pair<std::optional<Token>, std::optional<CompilationError>>
Tokenizer::nextToken() {
  // 用于存储已经读到的组成当前token字符
  std::stringstream ss;
  // 分析token的结果，作为此函数的返回值
  std::pair<std::optional<Token>, std::optional<CompilationError>> result;
  // <行号，列号>，表示当前token的第一个字符在源代码中的位置
  std::pair<int64_t, int64_t> pos;
  // 记录当前自动机的状态，进入此函数时是初始状态
  DFAState current_state = DFAState::INITIAL_STATE;
  DFAState next_state;

  // 这是一个死循环，除非主动跳出
  // 每一次执行while内的代码，都可能导致状态的变更
  while (true) {
    // 读一个字符，请注意auto推导得出的类型是std::optional<char>
    // 这里其实有两种写法
    // 1. 每次循环前立即读入一个 char
    // 2. 只有在可能会转移的状态读入一个 char
    // 因为我们实现了 unread，为了省事我们选择第一种

    auto current_char = nextChar();

    // 遇到了文件尾：返回一个空的token，和编译错误ErrEOF
    if (!current_char.has_value()) {
      return std::make_pair(std::optional<Token>(),
                            std::make_optional<CompilationError>(0, 0, ErrEOF));
    }
    char ch = current_char.value();

    // TODO : 错误处理

    next_state = nextState(current_state, ch);
    // 如果发生了从INITIAL到其他状态的转换
    // ss接受字符
    if (current_state == DFAState::INITIAL_STATE &&
        next_state != DFAState::INITIAL_STATE) {
      pos = previousPos();
    }
    if (next_state != DFAState::INITIAL_STATE) {
      ss << ch;
    } else if (next_state == DFAState::INITIAL_STATE &&
               current_state != DFAState::INITIAL_STATE) {
      // 任何一个状态结束后都会重新返回到初始状态
      // 如果返回到初始状态，说明已经处理完了一个Token
      // 故可以返回结果
      unreadLast();
      std::string stoken;
      ss >> stoken;

      auto tokentype = StateToTokenType[current_state];
      // 特判标识符
      if (current_state == DFAState::IDENTIFIER_STATE) {
        tokentype = verifyKeyword(stoken);
      }
      return std::make_pair(
          std::make_optional<Token>(tokentype, stoken, pos, currentPos()),
          std::optional<CompilationError>());
    }

    current_state = next_state;

    // switch (current_state) {
    //   default:
    //     DieAndPrint("unhandled state.");
    //     break;
    // }
  }
  // 预料之外的状态，如果执行到了这里，说明程序异常
  return std::make_pair(std::optional<Token>(),
                        std::optional<CompilationError>());
}

std::optional<CompilationError> Tokenizer::checkToken(const Token& t) {
  switch (t.GetType()) {
    case IDENTIFIER: {
      auto val = t.GetValueString();
      if (miniplc0::isdigit(val[0]))
        return std::make_optional<CompilationError>(
            t.GetStartPos().first, t.GetStartPos().second,
            ErrorCode::ErrInvalidIdentifier);
      break;
    }
    default:
      break;
  }
  return {};
}

void Tokenizer::readAll() {
  if (_initialized) return;
  for (std::string tp; std::getline(_rdr, tp);)
    _lines_buffer.emplace_back(std::move(tp + "\n"));
  _initialized = true;
  _ptr = std::make_pair<int64_t, int64_t>(0, 0);
  return;
}

// Note: We allow this function to return a postion which is out of bound
// according to the design like std::vector::end().
std::pair<uint64_t, uint64_t> Tokenizer::nextPos() {
  if (_ptr.first >= _lines_buffer.size()) DieAndPrint("advance after EOF");
  if (_ptr.second == _lines_buffer[_ptr.first].size() - 1)
    return std::make_pair(_ptr.first + 1, 0);
  else
    return std::make_pair(_ptr.first, _ptr.second + 1);
}

std::pair<uint64_t, uint64_t> Tokenizer::currentPos() { return _ptr; }

std::pair<uint64_t, uint64_t> Tokenizer::previousPos() {
  if (_ptr.first == 0 && _ptr.second == 0)
    DieAndPrint("previous position from beginning");
  if (_ptr.second == 0)
    return std::make_pair(_ptr.first - 1,
                          _lines_buffer[_ptr.first - 1].size() - 1);
  else
    return std::make_pair(_ptr.first, _ptr.second - 1);
}

std::optional<char> Tokenizer::nextChar() {
  if (isEOF()) return {};  // EOF
  auto result = _lines_buffer[_ptr.first][_ptr.second];
  _ptr = nextPos();
  return result;
}

bool Tokenizer::isEOF() { return _ptr.first >= _lines_buffer.size(); }

// Note: Is it evil to unread a buffer?
void Tokenizer::unreadLast() { _ptr = previousPos(); }

enum TokenType Tokenizer::verifyKeyword(std::string keyword) {
  if (keyword == "int") return TokenType::INT;
  if (keyword == "double") return TokenType::DOUBLE;
  if (keyword == "void") return TokenType::VOID;
  if (keyword == "fn") return TokenType::FN;
  if (keyword == "let") return TokenType::LET;
  if (keyword == "const") return TokenType::CONST;
  if (keyword == "as") return TokenType::AS;
  if (keyword == "while") return TokenType::WHILE;
  if (keyword == "if") return TokenType::IF;
  if (keyword == "else") return TokenType::ELSE;
  if (keyword == "break") return TokenType::BREAK;
  if (keyword == "continue") return TokenType::CONTINUE;
  if (keyword == "return") return TokenType::RETURN;
  return TokenType::IDENTIFIER;
}

DFAState Tokenizer::nextState(DFAState& current_state, char c) {
  auto next_state = dfaStateMachine[{current_state, c}];
  return next_state;
}

std::optional<CompilationError> Tokenizer::addDfaEdge(DFAState before,
                                                      std::string charSet,
                                                      DFAState after) {
  for (char& c : charSet) {
    dfaStateMachine.insert({{before, c}, after});
  }
  return {};
}

std::optional<CompilationError> Tokenizer::initDfaStateMachine() {
  // INITIAL_STATE
  addDfaEdge(DFAState::INITIAL_STATE, "=", DFAState::ASSIGN_STATE);
  addDfaEdge(DFAState::INITIAL_STATE, "-", DFAState::MINUS_SIGN_STATE);
  addDfaEdge(DFAState::INITIAL_STATE, "+", DFAState::PLUS_SIGN_STATE);
  addDfaEdge(DFAState::INITIAL_STATE, "*", DFAState::MULT_SIGN_STATE);
  addDfaEdge(DFAState::INITIAL_STATE, "/", DFAState::DIV_SIGN_STATE);
  addDfaEdge(DFAState::INITIAL_STATE, "(", DFAState::LEFT_BRACKET_STATE);
  addDfaEdge(DFAState::INITIAL_STATE, ")", DFAState::RIGHT_BRACKET_STATE);
  addDfaEdge(DFAState::INITIAL_STATE, "{", DFAState::LEFT_BRACE_STATE);
  addDfaEdge(DFAState::INITIAL_STATE, "}", DFAState::RIGHT_BRACE_STATE);
  addDfaEdge(DFAState::INITIAL_STATE, ":", DFAState::COLON_STATE);
  addDfaEdge(DFAState::INITIAL_STATE, ";", DFAState::SEMICOLON_STATE);
  addDfaEdge(DFAState::INITIAL_STATE, ",", DFAState::COMMA_STATE);
  addDfaEdge(DFAState::INITIAL_STATE, "<", DFAState::LESS_SIGN_STATE);
  addDfaEdge(DFAState::INITIAL_STATE, ">", DFAState::GREATER_SIGN_STATE);

  addDfaEdge(DFAState::INITIAL_STATE, "!", DFAState::NO_EQUAL_STATE);
  addDfaEdge(DFAState::NO_EQUAL_STATE, "=", DFAState::NO_EQUAL_STATE);

  addDfaEdge(DFAState::ASSIGN_STATE, "=", DFAState::EQUAL_STATE);

  addDfaEdge(DFAState::LESS_SIGN_STATE, "=", DFAState::LESS_EQUAL_STATE);

  addDfaEdge(DFAState::GREATER_SIGN_STATE, "=", DFAState::GREATER_EQUAL_STATE);

  addDfaEdge(DFAState::MINUS_SIGN_STATE, ">", DFAState::ARROW_STATE);

  addDfaEdge(DFAState::INITIAL_STATE,
             "_abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ",
             DFAState::IDENTIFIER_STATE);
  addDfaEdge(DFAState::IDENTIFIER_STATE,
             "_abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ",
             DFAState::IDENTIFIER_STATE);

  addDfaEdge(DFAState::INITIAL_STATE, "0123456789",
             DFAState::UNSIGNED_INTEGER_STATE);
  addDfaEdge(DFAState::UNSIGNED_INTEGER_STATE, "0123456789",
             DFAState::UNSIGNED_INTEGER_STATE);

  addDfaEdge(DFAState::UNSIGNED_INTEGER_STATE, ".",
             DFAState::UNSIGNED_DOUBLE_STATE);
  addDfaEdge(DFAState::UNSIGNED_DOUBLE_STATE, "0123456789",
             DFAState::UNSIGNED_DOUBLE_STATE);

  // TODO: String
  addDfaEdge(DFAState::INITIAL_STATE, "\"", DFAState::STRING_LITERAL_STATE);
  // TODO: Char

  return {};
}

std::optional<CompilationError> Tokenizer::initStateToTokenType() {
  StateToTokenType.insert(
      {DFAState::UNSIGNED_INTEGER_STATE, TokenType::UNSIGNED_INTEGER});
  StateToTokenType.insert(
      {DFAState::UNSIGNED_DOUBLE_STATE, TokenType::UNSIGNED_DOUBLE});
  StateToTokenType.insert({DFAState::PLUS_SIGN_STATE, TokenType::PLUS_SIGN});
  StateToTokenType.insert({DFAState::MINUS_SIGN_STATE, TokenType::MINUS_SIGN});
  StateToTokenType.insert({DFAState::DIV_SIGN_STATE, TokenType::DIV_SIGN});
  StateToTokenType.insert({DFAState::MULT_SIGN_STATE, TokenType::MULT_SIGN});
  StateToTokenType.insert({DFAState::IDENTIFIER_STATE, TokenType::IDENTIFIER});
  StateToTokenType.insert({DFAState::ASSIGN_STATE, TokenType::ASSIGN});
  StateToTokenType.insert({DFAState::EQUAL_STATE, TokenType::EQUAL});
  StateToTokenType.insert({DFAState::NO_EQUAL_STATE, TokenType::NO_EQUAL});
  StateToTokenType.insert({DFAState::LESS_SIGN_STATE, TokenType::LESS_SIGN});
  StateToTokenType.insert(
      {DFAState::GREATER_SIGN_STATE, TokenType::GREATER_SIGN});
  StateToTokenType.insert({DFAState::LESS_EQUAL_STATE, TokenType::LESS_EQUAL});
  StateToTokenType.insert(
      {DFAState::GREATER_EQUAL_STATE, TokenType::GREATER_EQUAL});
  StateToTokenType.insert(
      {DFAState::LEFT_BRACKET_STATE, TokenType::LEFT_BRACKET});
  StateToTokenType.insert(
      {DFAState::RIGHT_BRACKET_STATE, TokenType::RIGHT_BRACKET});
  StateToTokenType.insert({DFAState::LEFT_BRACE_STATE, TokenType::LEFT_BRACE});
  StateToTokenType.insert(
      {DFAState::RIGHT_BRACE_STATE, TokenType::RIGHT_BRACE});
  StateToTokenType.insert({DFAState::ARROW_STATE, TokenType::ARROW});
  StateToTokenType.insert({DFAState::COMMA_STATE, TokenType::COMMA});
  StateToTokenType.insert({DFAState::COLON_STATE, TokenType::COLON});
  StateToTokenType.insert({DFAState::SEMICOLON_STATE, TokenType::SEMICOLON});
  return {};
}

}  // namespace miniplc0
