#include "ast/ast.h"
#include "generator.h"

namespace miniplc0 {

void Generator::generateProgram(ProgNodePtr prog_node) {
  for (auto &func : prog_node->_funcs) {
    generateFunction(func);
  }
  show();
}

}  // namespace miniplc0
