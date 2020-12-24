#include "analyser.h"

#include <climits>

namespace miniplc0 {

NodePtr Analyser::Analyse() {
  try {
    auto node = analyseProgram();
    return node;
  } catch (const ErrorCode& ecode) {
    throw AnalyserError(_current_pos, ecode);
  }
}

// 返回下一个Token
optional<Token> Analyser::nextToken() {
  if (_offset == _tokens.size()) return {};
  // 考虑到 _tokens[0..._offset-1] 已经被分析过了
  // 所以我们选择 _tokens[0..._offset-1] 的 EndPos 作为当前位置
  _current_pos = _tokens[_offset].GetEndPos();
  // 自动忽略comment
  while (_tokens[_offset].GetType() == TokenType::COMMENT) ++_offset;
  return _tokens[_offset++];
}

optional<TokenType> Analyser::peekATokenGetType() {
  if (_offset == _tokens.size()) return TokenType::NULL_TOKEN;
  return _tokens[_offset].GetType();
}

// 回退上一个Token
void Analyser::unreadToken() {
  if (_offset == 0) DieAndPrint("analyser unreads token from the begining.");
  // 自动忽略comment
  while (_tokens[_offset - 1].GetType() == TokenType::COMMENT) --_offset;
  _current_pos = _tokens[_offset - 1].GetEndPos();
  _offset--;
}

}  // namespace miniplc0