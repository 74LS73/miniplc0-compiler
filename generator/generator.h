#pragma once

#include <optional>
#include <utility>
#include <vector>

#include "ast/ast.h"
#include "instruction/instruction.h"
#include "symbol/type.h"

namespace miniplc0 {

using std::vector;

class Generator final {
 public:
  Generator() = default;
  void operator+=(Generator &);
  int64_t size() { return _instructions.size(); }

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
  void generateBr(int64_t num);
  void generateBrTrue(int64_t num);
  void generateBreak();
  void generateContinue();
  void fixBreakAndContinue();

 private:
  std::vector<Instruction> _instructions;
  FuncNodePtr _cur_func;
  std::vector<Instruction> _cur_block;

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
};

}  // namespace miniplc0
