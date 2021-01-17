#include <memory>

#include "symbol_table.h"
namespace miniplc0 {

FuncNodePtr getGetintFunction() {
  auto func = std::make_shared<FuncNode>();
  func->_name = "getint";
  func->_param_slots = 0;
  func->_return_type = TokenType::INT;
  func->_return_slots = 1;
  func->_is_std = true;
  return func;
}

FuncNodePtr getGetdoubleFunction() {
  auto func = std::make_shared<FuncNode>();
  func->_name = "getdouble";
  func->_param_slots = 0;
  func->_return_type = TokenType::INT;
  func->_return_slots = 1;
  func->_is_std = true;
  return func;
}

FuncNodePtr getGetcharFunction() {
  auto func = std::make_shared<FuncNode>();
  func->_name = "getchar";
  func->_param_slots = 0;
  func->_return_type = TokenType::INT;
  func->_return_slots = 1;
  func->_is_std = true;
  return func;
}

FuncNodePtr getPutintFunction() {
  auto func = std::make_shared<FuncNode>();
  func->_name = "putint";
  func->_param_slots = 1;
  auto p = std::make_shared<DeclStatNode>();
  p->_type = TokenType::INT;
  p->_const = false;
  func->_params.emplace_back(p);
  func->_return_type = TokenType::VOID;
  func->_return_slots = 0;
  func->_is_std = true;
  return func;
}

FuncNodePtr getPutdoubleFunction() {
  auto func = std::make_shared<FuncNode>();
  func->_name = "putdouble";
  func->_param_slots = 1;
  auto p = std::make_shared<DeclStatNode>();
  p->_type = TokenType::DOUBLE;
  p->_const = false;
  func->_params.emplace_back(p);
  func->_return_type = TokenType::VOID;
  func->_return_slots = 0;
  func->_is_std = true;
  return func;
}
FuncNodePtr getPutcharFunction() {
  auto func = std::make_shared<FuncNode>();
  func->_name = "putchar";
  func->_param_slots = 1;
  auto p = std::make_shared<DeclStatNode>();
  p->_type = TokenType::INT;
  p->_const = false;
  func->_params.emplace_back(p);
  func->_return_type = TokenType::VOID;
  func->_return_slots = 0;
  func->_is_std = true;
  return func;
}
FuncNodePtr getPutstrFunction() {
  auto func = std::make_shared<FuncNode>();
  func->_name = "putstr";
  func->_param_slots = 1;
  auto p = std::make_shared<DeclStatNode>();
  p->_type = TokenType::INT;
  p->_const = false;
  func->_params.emplace_back(p);
  func->_return_type = TokenType::VOID;
  func->_return_slots = 0;
  func->_is_std = true;
  return func;
}

FuncNodePtr getPutlnFunction() {
  auto func = std::make_shared<FuncNode>();
  func->_name = "putln";
  func->_param_slots = 0;
  func->_return_type = TokenType::VOID;
  func->_return_slots = 0;
  func->_is_std = true;
  return func;
}

// 检查是否为标准库函数
FuncNodePtr SymbolTable::getStandardFunctionByName(const std::string &s) {
  FuncNodePtr func = nullptr;
  if (s == "getint") func = getGetintFunction();
  else if (s == "getdouble") func = getGetdoubleFunction();
  else if (s == "getchar") func = getGetcharFunction();
  else if (s == "putint") func = getPutintFunction();
  else if (s == "putdouble") func = getPutdoubleFunction();
  else if (s == "putchar") func = getPutcharFunction();
  else if (s == "putstr") func = getPutstrFunction();
  else if (s == "putln") func = getPutlnFunction();
  else throw ErrorCode::ErrNeedDeclareSymbol;
  std::cout << "fitst " << func->_name << std::endl;
  addFunction(func);
  return func;
}

}  // namespace miniplc0