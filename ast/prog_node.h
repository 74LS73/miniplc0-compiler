#pragma once

#include <any>
#include <cstdint>
#include <memory>  // for std::shared_ptr
#include <string>

#include "symbol/type.h"
#include "func_node.h"

namespace miniplc0 {

using std::shared_ptr;
using std::vector;




class ProgNode : public Node{
 public:
  vector<shared_ptr<StatNode>> _vars;
  vector<shared_ptr<FuncNode>> _funcs;
  ProgNode() { _ntype = NodeType::ProgNode; }
};

typedef std::shared_ptr<ProgNode> ProgNodePtr;

}  // namespace miniplc0
