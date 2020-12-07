#include "symbol_table.h"

namespace miniplc0 {

template<typename T>
void SymbolTable::_add(const Token &tk, std::map<std::string, T> &mp, T item, int &_nextTokenIndex) {
  if (tk.GetType() != TokenType::IDENTIFIER)
    DieAndPrint("only identifier can be added to the table.");
  mp[tk.GetValueString()] = item;
  _nextTokenIndex++;
}

void SymbolTable::addVariable(const Token &tk, VariableItem item) { _add(tk, _vars, item, _nextFunctionIndex); }

void SymbolTable::addFunction(const Token &tk, FunctionItem item) { _add(tk, _function, item, _nextFunctionIndex); }

bool SymbolTable::hasFunction(const std::string &s) {
  return _function.count(s);
}

bool SymbolTable::hasVariable(const std::string &s) {
  return _vars.count(s);
}

bool SymbolTable::isDeclared(const std::string &s) {
  return _vars.count(s) + _function.count(s); 
}

bool SymbolTable::isConstant(const std::string &s) {
  auto item = _vars[s];
  return item.is_const;
}

int SymbolTable::getVariableNumber() { return _vars.size(); }


}  // namespace miniplc0