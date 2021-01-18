#pragma once

#include <optional>
#include <stack>
#include <utility>
#include <vector>

#include "ast/ast.h"
#include "instruction/instruction.h"
#include "symbol/type.h"

namespace miniplc0 {

using std::vector;

class Generator final {
 public:
  Generator() = delete;
  Generator(std::ostream &output) { _output = &output; };
  // void operator+=(Generator &);
  int64_t size() { return instructions.size(); }

 public:
  void generateProgram(ProgNodePtr);

 private:
  void generateOperation(TokenType type, TokenType op);
  void show();
  void generateNega(TokenType type);
  void generateAs(TokenType type, TokenType as);
  void generateLoadVariable(int id, VariableType vt);
  void generateGetVariable(int id, VariableType type);
  void generateLiteralValue(int64_t *);
  void generateStackAlloc(int64_t size);
  void generateStore();
  void generateRet();
  void generateCallFunction(int64_t id);
  void generateCallName(int64_t global_index);
  void generateBr(int64_t num);
  void generateBrTrue(int64_t num);
  void generateBreak();
  void generateContinue();
  void fixBreakAndContinue();
  bool isReturned();

 private:
  std::ostream *_output;
  FuncNodePtr _cur_func;
  std::stack<vector<Instruction>> _code_stack;

 public:
  std::vector<Instruction> instructions;

 private:
  void generateFunction(FuncNodePtr);
  // vector<VariableItem> &generateFunctionParam(FuncNodePtr);

  void generateStat(StatNodePtr);
  void generateExprStat(ExprStatNodePtr);
  void generateBlockStat(BlockStatNodePtr);
  void generateDeclStat(DeclStatNodePtr);
  void generateIfStat(IfStatNodePtr);
  void generateWhileStat(WhileStatNodePtr);
  void generateBreakStat(BreakStatNodePtr);
  void generateContinueStat(ContinueStatNodePtr);
  void generateReturnStat(ReturnStatNodePtr);
  void generateWhileStat(BlockStatNodePtr);

  void generateExpr(ExprNodePtr);
  void generateAssignExpr(AssignExprNodePtr);
  void generateUnaryExpr(UnaryExprNodePtr);
  void generateOpExpr(OpExprNodePtr);
  void generateCallExpr(CallExprNodePtr);
  void generateItemExpr(ItemExprNodePtr);
  void generateIdentExpr(IdentExprNodePtr);

 private:
  void _Write(const uint64_t &, size_t);
};

}  // namespace miniplc0
