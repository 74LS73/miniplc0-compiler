#include "ast/ast.h"
#include "generator.h"

namespace miniplc0 {

// dfsa

void Generator::generateFunction(FuncNodePtr func_node) {
  auto func = FunctionItem();
  // _symbol_table_stack.declareFunction(func);

  // _symbol_table_stack.pushNextScope();
  func.return_type = func_node->_return_type;
  // func.params = generateFunctionParam(func_node);

  auto block = std::dynamic_pointer_cast<BlockStatNode>(func_node->_body);
  generateBlockStat(block);

  // _symbol_table_stack.popCurrentScope();
}

// vector<VariableItem>& Generator::generateFunctionParam(FuncNodePtr func_node) {
//   auto params = vector<VariableItem>();
//   if (func_node->_return_type != TokenType::VOID) {
//     auto param = VariableItem();
//     // _symbol_table_stack.declareVariable(param);
//     params.emplace_back(param);
//   }
//   for (auto var : func_node->_params) {
//     auto param = VariableItem();
//     // _symbol_table_stack.declareVariable(param);
//     params.emplace_back(param);
//   }
//   return params;
// }


}  // namespace miniplc0
