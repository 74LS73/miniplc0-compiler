#pragma once

#include <any>
#include <cstdint>
#include <memory>  // for std::shared_ptr
#include <string>
#include <vector>

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
  int32_t _return_slots = 0;
  int32_t _param_slots = 0;
  vector<DeclStatNodePtr> _locs;
  int32_t _loc_slots = 0;
  bool _is_std;
  bool _need_ret;
  int32_t _global_index;
  BlockStatNodePtr _body;
  int64_t _id;
  FuncNode() : _is_std(false), _need_ret(true) { _ntype = NodeType::FuncNode; }
};

typedef std::shared_ptr<FuncNode> FuncNodePtr;

}  // namespace miniplc0
