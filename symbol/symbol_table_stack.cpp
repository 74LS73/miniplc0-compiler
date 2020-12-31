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
bool SymbolTableStack::isLocalVariableDeclared(const string &token_name) {
  return getCurrentTable().hasVariable(token_name);
}

bool SymbolTableStack::isFunctionDeclared(const string &token_name) {
  if (_symbol_table_stack[_global_scope_level].hasFunction(token_name))
    return true;
  // 检查是否是标准库函数
  auto func = getStandardFunctionByName(token_name);
  if (!func.has_value()) return false;
  // _symbol_table_stack[_global_scope_level].addFunction(func.value());
  return true;
}

bool SymbolTableStack::isGlobalVariableDeclared(const string &token_name) {
  return _symbol_table_stack[_global_scope_level].hasVariable(token_name);
}

int64_t SymbolTableStack::declareVariable(DeclStatNodePtr item) {
  getCurrentTable().addVariable(item);
}

int64_t SymbolTableStack::declareGlobalVariable(DeclStatNodePtr item) {
  getCurrentTable().addVariable(item);
}

int64_t SymbolTableStack::declareFunction(FuncNodePtr item) {
  _symbol_table_stack[0].addFunction(item);
}

DeclStatNodePtr SymbolTableStack::getVariableByName(string s) {
  for (int i = _cur_scope_level; i >= 0; --i) {
    if (_symbol_table_stack[i].hasVariable(s)) {
      return _symbol_table_stack[i].getVariableByName(s);
    }
  }
  throw ErrorCode::ErrNeedDeclareSymbol;
}

DeclStatNodePtr SymbolTableStack::getGlobalVariableByName(string s) {
  return _symbol_table_stack[0].getVariableByName(s);
}

FuncNodePtr SymbolTableStack::getFunctionByName(string s) {
  return _symbol_table_stack[0].getFunctionByName(s);
}

}  // namespace miniplc0
