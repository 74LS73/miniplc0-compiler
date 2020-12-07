#include "symbol.h"

#include <climits>

#include "analyser.h"

namespace miniplc0 {
// 此处汇聚了symbol的相关函数
// 以及analyser中有关symbol的函数

template<typename T>
void Symbol::_add(const Token &tk, std::map<std::string, T> &mp, T item, int &_nextTokenIndex) {
  if (tk.GetType() != TokenType::IDENTIFIER)
    DieAndPrint("only identifier can be added to the table.");
  mp[tk.GetValueString()] = item;
  _nextTokenIndex++;
}

void Symbol::addVariable(const Token &tk, VariableItem item) { _add(tk, _vars, item, _nextFunctionIndex); }

void Symbol::addFunction(const Token &tk, FunctionItem item) { _add(tk, _function, item, _nextFunctionIndex); }

bool Symbol::hasFunction(const std::string &s) {
  return _function.count(s);
}

bool Symbol::hasVariable(const std::string &s) {
  return _vars.count(s);
}

bool Symbol::isDeclared(const std::string &s) {
  return _vars.count(s) + _function.count(s); 
}

bool Symbol::isConstant(const std::string &s) {
  auto item = _vars[s];
  return item.is_const;
}

int Symbol::getVariableNumber() { return _vars.size(); }





void Analyser::_initTableStack() {
  _table_stack.push_back({});
}


int32_t Analyser::getIndex(const std::string &s) {
  if (_uninitialized_vars.find(s) != _uninitialized_vars.end())
    return _uninitialized_vars[s];
  else if (_vars.find(s) != _vars.end())
    return _vars[s];
  else
    return _consts[s];
}



// 加入一层table
void Analyser::pushNextScope() {
  _table_stack.push_back(Symbol());
  ++_cur_scope_level;
}

// 删除当前table
void Analyser::popCurrentScope() { _table_stack.erase(_table_stack.end() - 1); }

// 获取当前table
Symbol Analyser::getCurrentTable() { return _table_stack[_cur_scope_level]; }

int Analyser::getCurrentScopeLevel() { return _cur_scope_level; }

// 是否被声明过
bool Analyser::isLocalVariableDeclared(const std::string &token_name) {
  return _table_stack[_cur_scope_level].hasVariable(token_name);
}

bool Analyser::isFunctionDeclared(const std::string &token_name) {
  return _table_stack[_global_scope_level].hasFunction(token_name);
}

bool Analyser::isGlobalVariableDeclared(const std::string &token_name) {
  return _table_stack[_global_scope_level].hasVariable(token_name);
}

void Analyser::declareVariable(const Token &tk, VariableItem &item) {
  getCurrentTable().addVariable(tk, item);
}

void Analyser::declareFunction(const Token &tk, FunctionItem &item) {
  getCurrentTable().addFunction(tk, item);
}

}  // namespace miniplc0