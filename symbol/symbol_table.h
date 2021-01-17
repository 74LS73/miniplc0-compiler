#pragma once

#include <map>
#include <optional>
#include <vector>

#include "ast/func_node.h"
#include "tokenizer/token.h"
#include "type.h"

namespace miniplc0 {

using std::string;

class SymbolTable final {
 public:
  SymbolTable() : _nextFunctionIndex(0), _nextVariableIndex(0){};

 private:
  int64_t _nextFunctionIndex;
  int64_t _nextVariableIndex;
  std::map<std::string, DeclStatNodePtr> _vars;
  std::map<std::string, FuncNodePtr> _function;

  // helper function
  template <typename T>
  void _add(std::map<string, T> &, string &, T, int64_t &);
  FuncNodePtr getStandardFunctionByName(const std::string &s);

 public:
  std::map<std::string, DeclStatNodePtr> getVars() { return _vars; }
  std::map<std::string, FuncNodePtr> getFuncs() { return _function; }
  int64_t addVariable(DeclStatNodePtr);
  int64_t addFunction(FuncNodePtr);

  bool hasFunction(const string &);
  bool hasVariable(const string &);

  DeclStatNodePtr getVariableByName(string &s);
  FuncNodePtr getFunctionByName(string &s);

  // 是否被声明过
  bool isDeclared(const string &);
  // 是否是常量
  bool isConstant(const string &);

  int getVariableNumber() { return _vars.size(); }
  int getFunctionNumber() { return _function.size(); }
};

}  // namespace miniplc0