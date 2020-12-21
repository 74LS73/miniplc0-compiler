#include "symbol_table_stack.h"

namespace miniplc0 {

FunctionItem getGetintFunction() {
  auto func = FunctionItem();
  func.name = "getint";
  func.param_slots = 0;
  func.return_type = TokenType::INT;
  func.return_slots = 1;
  return func;
}

FunctionItem getGetdoubleFunction() {
  auto func = FunctionItem();
  func.name = "getdouble";
  func.param_slots = 0;
  func.return_type = TokenType::INT;
  func.return_slots = 1;
  return func;
}

FunctionItem getGetcharFunction() {
  auto func = FunctionItem();
  func.name = "getchar";
  func.param_slots = 0;
  func.return_type = TokenType::INT;
  func.return_slots = 1;
  return func;
}

FunctionItem getPutintFunction() {
  auto func = FunctionItem();
  func.name = "putint";
  func.param_slots = 1;
  func.return_type = TokenType::VOID;
  func.return_slots = 0;
  return func;
}

FunctionItem getPutdoubleFunction() {
  auto func = FunctionItem();
  func.name = "putdouble";
  func.param_slots = 1;
  func.return_type = TokenType::VOID;
  func.return_slots = 0;
  return func;
}
FunctionItem getPutcharFunction() {
  auto func = FunctionItem();
  func.name = "putchar";
  func.param_slots = 1;
  func.return_type = TokenType::VOID;
  func.return_slots = 0;
  return func;
}
FunctionItem getPutstrFunction() {
  auto func = FunctionItem();
  func.name = "putstr";
  func.param_slots = 1;
  func.return_type = TokenType::VOID;
  func.return_slots = 0;
  return func;
}

FunctionItem getPutlnFunction() {
  auto func = FunctionItem();
  func.name = "putln";
  func.param_slots = 0;
  func.return_type = TokenType::VOID;
  func.return_slots = 0;
  return func;
}

// 检查是否为标准库函数
std::optional<FunctionItem> SymbolTableStack::getStandardFunctionByName(
    const std::string &s) {
  if (s == "getint") return getGetintFunction();
  if (s == "getdouble") return getGetdoubleFunction();
  if (s == "getchar") return getGetcharFunction();
  if (s == "putint") return getPutintFunction();
  if (s == "putdouble") return getPutdoubleFunction();
  if (s == "putchar") return getPutcharFunction();
  if (s == "putstr") return getPutstrFunction();
  if (s == "putln") return getPutlnFunction();
  return {};
}

}  // namespace miniplc0