#include "analyser.h"

#include <climits>

namespace miniplc0 {

std::pair<std::vector<Instruction>, std::optional<CompilationError>>
Analyser::Analyse() {
  auto err = analyseProgram();
  if (err.has_value())
    return std::make_pair(std::vector<Instruction>(), err);
  else
    return std::make_pair(_instructions, std::optional<CompilationError>());
}

// 返回下一个Token
std::optional<Token> Analyser::nextToken() {
  if (_offset == _tokens.size()) return {};
  // 考虑到 _tokens[0..._offset-1] 已经被分析过了
  // 所以我们选择 _tokens[0..._offset-1] 的 EndPos 作为当前位置
  _current_pos = _tokens[_offset].GetEndPos();
  // 自动忽略comment
  while (_tokens[_offset].GetType() == TokenType::COMMENT)
    ++_offset;
  return _tokens[_offset++];
}

// 回退上一个Token
void Analyser::unreadToken() {
  if (_offset == 0) DieAndPrint("analyser unreads token from the begining.");
  // 自动忽略comment
  while (_tokens[_offset - 1].GetType() == TokenType::COMMENT)
    --_offset;
  _current_pos = _tokens[_offset - 1].GetEndPos();
  _offset--;
}

}  // namespace miniplc0