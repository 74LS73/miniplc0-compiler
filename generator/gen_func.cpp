#include "ast/ast.h"
#include "generator.h"

namespace miniplc0 {

void Generator::generateFunction(FuncNodePtr func_node) {
  printf("==== begin %s! ====\n", func_node->_name.c_str());
  _code_stack.push(vector<Instruction>());
  _cur_func = func_node;
  auto block = func_node->_body;
  generateBlockStat(block);
  _cur_func = nullptr;

  auto func_block = _code_stack.top();
  _code_stack.pop();
  auto &_base_block = _code_stack.top();
  for (auto &isa : func_block) {
    _base_block.emplace_back(isa);
  }
}

// vector<VariableItem>& Generator::generateFunctionParam(FuncNodePtr func_node)
// {
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
