#include "symbol_table.h"

namespace miniplc0 {

template <typename T>
void SymbolTable::_add(std::map<std::string, T> &mp,
                       T item, int &_nextTokenIndex) {
  mp[item.name] = item;
  _nextTokenIndex++;
}

void SymbolTable::addVariable(VariableItem item) {
  _add(_vars, item, _nextFunctionIndex);
}

void SymbolTable::addFunction(FunctionItem item) {
  _add(_function, item, _nextFunctionIndex);
}

bool SymbolTable::hasFunction(const std::string &s) {
  return _function.count(s);
}

bool SymbolTable::hasVariable(const std::string &s) { return _vars.count(s); }

bool SymbolTable::isDeclared(const std::string &s) {
  return _vars.count(s) + _function.count(s);
}

bool SymbolTable::isConstant(const std::string &s) {
  auto item = _vars[s];
  return item.is_const;
}

int SymbolTable::getVariableNumber() { return _vars.size(); }

std::optional<VariableItem> SymbolTable::getVariableByName(std::string &s) {
  if (hasVariable(s)) {
    return std::make_optional(_vars[s]);
  }
  return {};
}

std::optional<FunctionItem> SymbolTable::getFunctionByName(std::string &s) {
  if (hasFunction(s)) {
    return std::make_optional(_function[s]);
  }
  return {};
}

}  // namespace miniplc0
