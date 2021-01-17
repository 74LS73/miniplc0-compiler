#include "ast/ast.h"
#include "generator.h"

namespace miniplc0 {

void Generator::generateProgram(ProgNodePtr prog_node) {
  _code_stack.push(vector<Instruction>());

  printf("magic: %x\n", prog_node->_magic);
  _Write(prog_node->_magic, 4);
  printf("version: %x\n", prog_node->_version);
  _Write(prog_node->_version, 4);

  printf("globals.count: %x\n",  prog_node->_globals->getVariableNumber() + prog_node->_globals->getFunctionNumber());
  _Write(prog_node->_globals->getVariableNumber() + prog_node->_globals->getFunctionNumber(), 4);


  int i = 0;
  for (auto &var : prog_node->_vars) {
    printf("is_const: %d\n", var->_const);
    _Write(var->_const, 1);
    printf("value.count: %d\n", 8);
    _Write(8, 4);
    printf("value.item: %d\n", 0);
    _Write(0, 8);
    i++;
  }

  for (auto &func : prog_node->_globals->getFuncs()) {
    func.second->_global_index = i;
    printf("global_index is %d\n", func.second->_global_index);
    printf("is_const: %d\n", 0);
    _Write(0, 1);
    printf("value.count: %d\n", func.first.size());
    _Write(func.first.size(), 4);
    printf("value.item: %s\n", func.first.c_str());
    for (auto &c : func.first) {
      _Write(c, 1);
    }
    i++;
  }

  printf("functions.count: %x\n", prog_node->_funcs.size());
  _Write(prog_node->_funcs.size(), 4);

  for (auto &func : prog_node->_funcs) {
    generateFunction(func);
  }

  auto code_block = _code_stack.top();
  for (auto &isa : code_block) {
    instructions.push_back(isa);
  }
}

}  // namespace miniplc0
