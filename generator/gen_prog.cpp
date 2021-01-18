#include "ast/ast.h"
#include "generator.h"

namespace miniplc0 {

void Generator::generateProgram(ProgNodePtr prog_node) {
  _code_stack.push(vector<Instruction>());

  printf("magic: %x\n", prog_node->_magic);
  _Write(prog_node->_magic, 4);
  printf("version: %x\n", prog_node->_version);
  _Write(prog_node->_version, 4);

  printf("globals.count: %x\n", prog_node->_globals->getVariableNumber() +
                                    prog_node->_globals->getFunctionNumber());
  _Write(prog_node->_globals->getVariableNumber() +
             prog_node->_globals->getFunctionNumber(),
         4);

  int global_index = 0;
  for (auto &var : prog_node->_globals->_vars) {
    printf("global_index is %d\n", global_index);
    printf("is_const: %d\n", var.second->_const);
    _Write(var.second->_const, 1);
    if (var.second->_type == TokenType::STRING_LITERAL) {
      printf("value.count: %d\n", var.second->_name.size());
      _Write(var.second->_name.size(), 4);
      printf("value.item: %s\n", var.second->_name.c_str());
      for (auto &c : var.second->_name) {
        _Write(c, 1);
      }
    } else {
      printf("value.count: %d\n", 8);
      _Write(8, 4);
      printf("value.item: %d\n", 0);
      _Write(0, 8);
    }
    global_index++;
  }
  
  int func_index = 0;
  for (auto &func : prog_node->_globals->getFuncs()) {
    func.second->_global_index = global_index;
    if (!func.second->_is_std) {
      func.second->_id = func_index ++;
    }
    printf("global_index is %d\n", func.second->_global_index);
    printf("is_const: %d\n", 0);
    _Write(0, 1);
    printf("value.count: %d\n", func.first.size());
    _Write(func.first.size(), 4);
    printf("value.item: %s\n", func.first.c_str());
    for (auto &c : func.first) {
      _Write(c, 1);
    }
    global_index++;
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
