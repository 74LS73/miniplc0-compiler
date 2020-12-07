#pragma once

#include <map>
#include <optional>
#include <vector>

#include "error/error.h"
#include "instruction/instruction.h"
#include "tokenizer/token.h"

namespace miniplc0 {

/**
 * 函数需要记录的信息：
 * 名字
 * 参数的个数
 * 内部局部变量的个数
 * 返回值的个数（0/1）
 */

// enum IdentifierType {
//   FUNCTION,
//   VARIABLE
// };

struct VariableItem {
  int32_t id;
  enum TokenType type;
  bool is_const;
};

struct FunctionItem {
  int32_t id;
  std::string name;
  int32_t param_slots;
  std::vector<VariableItem> params;
  int32_t return_slots;
  TokenType return_type;
  int32_t local_slots;
  std::vector<Instruction> body;
};

class SymbolTable final {
  
 public:
  SymbolTable(): _nextFunctionIndex(0), _nextVariableIndex(0) {};

 private:
  int32_t _nextFunctionIndex;
  int32_t _nextVariableIndex;
  std::map<std::string, VariableItem> _vars;
  std::map<std::string, FunctionItem> _function;
  
  // helper function
  template<typename T> 
  void _add(const Token &tk, std::map<std::string, T> &mp, T item, int &);
  
 public:
  void addVariable(const Token &tk, VariableItem item);
  void addFunction(const Token &tk, FunctionItem item);

  bool hasFunction(const std::string &);
  bool hasVariable(const std::string &);
  
  // 是否被声明过
  bool isDeclared(const std::string &);
  // 是否是常量
  bool isConstant(const std::string &);

  int getVariableNumber();
};

}  // namespace miniplc0