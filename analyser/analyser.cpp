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


std::optional<Token> Analyser::nextToken() {
  if (_offset == _tokens.size()) return {};
  // 考虑到 _tokens[0..._offset-1] 已经被分析过了
  // 所以我们选择 _tokens[0..._offset-1] 的 EndPos 作为当前位置
  _current_pos = _tokens[_offset].GetEndPos();
  return _tokens[_offset++];
}

void Analyser::unreadToken() {
  if (_offset == 0) DieAndPrint("analyser unreads token from the begining.");
  _current_pos = _tokens[_offset - 1].GetEndPos();
  _offset--;
}



// 以下下是针对int的操作
std::string removeNumberPrefixZero(std::string s) {
  int pos = s.find_first_not_of('0');
  return s.substr(pos, -1);
}

// 注意，此编译器接受的int统统为int64_t
bool Analyser::isIntegerOverflow(std::string s, int prefix) {
  if (s.length() < 20) return false;
  else if (s.length() >= 20) return true;
  else {
    if (s > "9223372036854775807") return true;
    else if (s == "9223372036854775808") return prefix != -1;
    else return false;
  }
}

}  // namespace miniplc0