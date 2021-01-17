#include "ast/ast.h"
#include "generator.h"

namespace miniplc0 {

void Generator::generateFunction(FuncNodePtr func_node) {
  printf("==== begin %s! ====\n", func_node->_name.c_str());
  //   00 00 00 01 // functions[0].name
  // 00 00 00 00 // functions[0].ret_slots
  // 00 00 00 00 // functions[0].param_slots
  // 00 00 00 00 // functions[0].loc_slots
  // 00 00 00 04 // functions[0].body.count

  printf("name: %d\n", func_node->_global_index);
  _Write(func_node->_global_index, 4);
  printf("ret_slots: %d\n", func_node->_return_slots);
  _Write(func_node->_return_slots, 4);
  printf("param_slots: %d\n", func_node->_param_slots);
  _Write(func_node->_param_slots, 4);
  printf("loc_slots: %d\n", func_node->_loc_slots);
  _Write(func_node->_loc_slots, 4);

  _code_stack.push(vector<Instruction>());
  _cur_func = func_node;
  auto block = func_node->_body;
  generateBlockStat(block);
  if (func_node->_need_ret) {
    generateRet();
  }
  _cur_func = nullptr;

  auto func_block = _code_stack.top();
  printf("body.count: %d\n", func_block.size());
  _Write(func_block.size(), 4);
  show();
  for (auto &isa : func_block) {
    isa.Write(*_output);
  }
  _code_stack.pop();
  auto &_base_block = _code_stack.top();
  for (auto &isa : func_block) {
    _base_block.emplace_back(isa);
  }

  return;
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
