#include "symbol_table.h"

namespace miniplc0 {

template <typename T>
void SymbolTable::_add(std::map<std::string, T> &mp, string &name, T item,
                       int64_t &_nextTokenIndex) {
  mp[name] = item;
  item->_id = _nextTokenIndex;
  _nextTokenIndex++;
}

int64_t SymbolTable::addVariable(DeclStatNodePtr decl) {
  std::string name = decl->_name;
  if (hasFunction(name)) {
    throw ErrorCode::ErrDuplicateDeclaration;
  }
  _add(_vars, name, decl, _nextVariableIndex);
  return decl->_id;
}

int64_t SymbolTable::addFunction(FuncNodePtr func) {
  std::string name = func->_name;
  if (hasFunction(name)) {
    throw ErrorCode::ErrDuplicateDeclaration;
  }
  _add(_function, name, func, _nextFunctionIndex);
  return func->_id;
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
  return item->_const;
}

DeclStatNodePtr SymbolTable::getVariableByName(std::string &s) {
  auto var = _vars[s];
  if (var != nullptr) return var;
  throw ErrorCode::ErrNeedDeclareSymbol;
}

FuncNodePtr SymbolTable::getFunctionByName(std::string &s) {
  if (_function.count(s))
    return _function[s];
  return getStandardFunctionByName(s);
}

}  // namespace miniplc0
