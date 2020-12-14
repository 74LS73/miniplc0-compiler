#pragma once

#include <map>
#include <optional>
#include <vector>

#include "ast/ast.h"
#include "instruction/instruction.h"
#include "tokenizer/token.h"
#include "generator/generator.h"



namespace miniplc0 {





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
  void _add(std::map<std::string, T> &mp, T item, int &);
  
 public:
  void addVariable(VariableItem item);
  void addFunction(FunctionItem item);

  bool hasFunction(const std::string &);
  bool hasVariable(const std::string &);

  std::optional<VariableItem> getVariableByName(std::string &s);
  std::optional<FunctionItem> getFunctionByName(std::string &s);

  // 是否被声明过
  bool isDeclared(const std::string &);
  // 是否是常量
  bool isConstant(const std::string &);

  int getVariableNumber();
};

}  // namespace miniplc0