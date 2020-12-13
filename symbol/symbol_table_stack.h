#pragma once

#include <map>
#include <optional>
#include <vector>

#include "symbol/symbol_table.h"

namespace miniplc0 {

#ifndef IN
#define IN
#endif

class SymbolTableStack final {
 public:
  SymbolTableStack() { _symbol_table_stack.push_back({}); };

 private:
  std::vector<SymbolTable> _symbol_table_stack;
  int _cur_scope_level = 0;
  const int _global_scope_level = 0;

 public:
  void pushNextScope();
  void popCurrentScope();

  bool isLocalVariableDeclared(const std::string &);
  bool isFunctionDeclared(const std::string &);
  bool isGlobalVariableDeclared(const std::string &);

  void declareVariable(const Token &tk, VariableItem &);
  void declareGlobalVariable(const Token &tk, VariableItem &);
  void declareFunction(const Token &tk, FunctionItem &);

  int getVariableNumber();
  std::optional<VariableItem> getVariableByName(std::string);
  std::optional<VariableItem> getGlobalVariableByName(std::string);
  std::optional<FunctionItem> getFunctionByName(std::string);
  int getCurrentScopeLevel();

 private:
  // 返回值和返回引用的区别了解一下
  SymbolTable &getCurrentTable();
};

}  // namespace miniplc0