#pragma once

#include <optional>
#include <utility>
#include <vector>

#include "instruction/instruction.h"
#include "symbol/type.h"
#include "tokenizer/token.h"

namespace miniplc0 {

class Generator final {
 public:
  Generator() = default;
  void operator+=(Generator &);
  int64_t size() { return _instructions.size(); }

 public:
  void generateOperation(TokenType type, TokenType op);
  void show();
  void generateNega(TokenType type);
  void generateAs(TokenType type, TokenType as);
  void generateLoadVariable(int id, VariableType vt);
  void generateGetVariable(int id, VariableType type);
  void generateDouble(double);
  void generateInt64(int64_t);
  void generateStackAlloc(int64_t size);
  void generateStore();
  void generateRet();
  void generateCallFunction(int64_t id);
  void generateBr(int64_t num);
  void generateBrTrue(int64_t num);

 private:
  std::vector<Instruction> _instructions;
};

}  // namespace miniplc0
