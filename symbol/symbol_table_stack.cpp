#include "symbol_table_stack.h"

namespace miniplc0 {

// 加入一层table
void SymbolTableStack::pushNextScope() {
  _symbol_table_stack.push_back(SymbolTable());
  ++_cur_scope_level;
}

// 删除当前table
void SymbolTableStack::popCurrentScope() {
  _symbol_table_stack.erase(_symbol_table_stack.end() - 1);
  --_cur_scope_level;
}

// 获取当前table
SymbolTable &SymbolTableStack::getCurrentTable() {
  return _symbol_table_stack[_cur_scope_level];
}

int SymbolTableStack::getCurrentScopeLevel() { return _cur_scope_level; }

// 是否被声明过
bool SymbolTableStack::isLocalVariableDeclared(const std::string &token_name) {
  return getCurrentTable().hasVariable(token_name);
}

bool SymbolTableStack::isFunctionDeclared(const std::string &token_name) {
  return _symbol_table_stack[_global_scope_level].hasFunction(token_name);
}

bool SymbolTableStack::isGlobalVariableDeclared(const std::string &token_name) {
  return _symbol_table_stack[_global_scope_level].hasVariable(token_name);
}

void SymbolTableStack::declareVariable(const Token &tk, VariableItem &item) {
  getCurrentTable().addVariable(tk, item);
}

void SymbolTableStack::declareGlobalVariable(const Token &tk,
                                             VariableItem &item) {
  getCurrentTable().addVariable(tk, item);
}

void SymbolTableStack::declareFunction(const Token &tk, FunctionItem &item) {
  getCurrentTable().addFunction(tk, item);
}

std::optional<VariableItem> SymbolTableStack::getVariableByName(
    std::string &s) {
  for (int i = _cur_scope_level; i >= 0; --i) {
    if (_symbol_table_stack[i].hasVariable(s)) {
      return _symbol_table_stack[i].getVariableByName(s);
    }
  }
  return {};
}

std::optional<VariableItem> SymbolTableStack::getGlobalVariableByName(
    std::string &s) {
  return _symbol_table_stack[0].getVariableByName(s);
}

std::optional<FunctionItem> SymbolTableStack::getFunctionByName(
    std::string &s) {
  return _symbol_table_stack[0].getFunctionByName(s);
}

}  // namespace miniplc0
