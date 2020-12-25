#include "ast/ast.h"
#include "generator.h"

namespace miniplc0 {


void Generator::generateFunction(FuncNodePtr func_node) {

  auto block = func_node->_body;
  generateBlockStat(block);

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
