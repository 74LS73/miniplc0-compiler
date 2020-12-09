#include "ast.h"

namespace miniplc0 {

void Item::combine(TokenType op, std::shared_ptr<miniplc0::Item> rhs) {
  this->p_code_gen += rhs->p_code_gen;
  this->p_code_gen.addOperation(this->type, op);
}

}  // namespace miniplc0
