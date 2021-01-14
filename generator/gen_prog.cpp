#include "ast/ast.h"
#include "generator.h"

namespace miniplc0 {

void Generator::generateProgram(ProgNodePtr prog_node) {
  _code_stack.push(vector<Instruction>());
  for (auto &func : prog_node->_funcs) {
    generateFunction(func);
  }
  show();
  auto code_block = _code_stack.top();
  for (auto &isa : code_block) {
    instructions.push_back(isa);
  }
}

}  // namespace miniplc0
