#pragma once

#include <map>
#include <optional>
#include <vector>

#include "error/error.h"
#include "instruction/instruction.h"
#include "symbol/symbol_table.h"
#include "tokenizer/token.h"

namespace miniplc0 {

class SymbolTableStack final {
 public:
  SymbolTableStack(){
    _symbol_table_stack.push_back({});
  };

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

 private:
  SymbolTable getCurrentTable();
  int getCurrentScopeLevel();
};

}  // namespace miniplc0