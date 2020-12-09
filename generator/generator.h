#pragma once

#include <utility>
#include <vector>
#include <optional>

#include "instruction/instruction.h"
#include "tokenizer/token.h"


namespace miniplc0 {

#ifndef VARIABLE_TYPE
#define VARIABLE_TYPE
enum VariableType {
  LOCAL,
  GLOBAL,
  PARAM
};
#endif

class Generator final {
 public:
  Generator() = default;
  void operator+=(Generator &);

 public:
  void addOperation(TokenType type, TokenType op);
  void show();
  void generateNega(TokenType type);
  void generateAs(TokenType type, TokenType as);
  void generateLoadVariable(int id, VariableType vt);
  void generateDouble(double);
  void generateInt64(int64_t);
  void generateStackAlloc(int64_t size);
  void generateCallFunction(int64_t id);

 private:
  std::vector<Instruction> _instructions;
};

}  // namespace miniplc0

