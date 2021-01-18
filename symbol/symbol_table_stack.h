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
  FuncNodePtr _function = nullptr;

 public:
  void pushNextScope();
  void popCurrentScope();
  void pushNextScopeWithIndex();
  void popCurrentScopeWithIndex();

  bool isLocalVariableDeclared(const std::string &);
  bool isFunctionDeclared(const std::string &);
  bool isGlobalVariableDeclared(const std::string &);

  int64_t declareVariable(DeclStatNodePtr);
  int64_t declareGlobalVariable(DeclStatNodePtr);
  int64_t declareFunction(FuncNodePtr);

  int getCurrentVariableNumber();
  DeclStatNodePtr getVariableByName(std::string);
  DeclStatNodePtr getGlobalVariableByName(std::string);
  FuncNodePtr getFunctionByName(std::string);
  int getCurrentScopeLevel();
  SymbolTable &getGlobalTable();
  SymbolTable &getCurrentTable();

  FuncNodePtr getCurrentFunction() { return _function; }
  void setCurrentFunction(FuncNodePtr func) { _function = func; }

 private:
  // 返回值和返回引用的区别了解一下

  // 检查是否为标准库函数
  // FuncNodePtr getStandardFunctionByName(const std::string &);
};

}  // namespace miniplc0