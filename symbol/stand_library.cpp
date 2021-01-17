#include <memory>

#include "symbol_table_stack.h"
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
FuncNodePtr SymbolTableStack::getStandardFunctionByName(const std::string &s) {
  if (s == "getint") return getGetintFunction();
  if (s == "getdouble") return getGetdoubleFunction();
  if (s == "getchar") return getGetcharFunction();
  if (s == "putint") return getPutintFunction();
  if (s == "putdouble") return getPutdoubleFunction();
  if (s == "putchar") return getPutcharFunction();
  if (s == "putstr") return getPutstrFunction();
  if (s == "putln") return getPutlnFunction();
  throw ErrorCode::ErrNeedDeclareSymbol;
}

}  // namespace miniplc0