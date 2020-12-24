#pragma once

#include <any>
#include <cstdint>
#include <memory>  // for std::shared_ptr
#include <vector>
#include <string>

#include "stat_node.h"
#include "symbol/type.h"
#include "tokenizer/token.h"

namespace miniplc0 {

using std::shared_ptr;
using std::vector;

class FuncNode : public Node {
 public:
  std::string _name;
  vector<DeclStatNodePtr> _params;
  TokenType _return_type;
  BlockStatNodePtr _body;
  FuncNode() { _ntype = NodeType::FuncNode; }
};

typedef std::shared_ptr<FuncNode> FuncNodePtr;

}  // namespace miniplc0
