#pragma once

#include <optional>
#include <utility>
#include <vector>

#include "analyser/analyser.h"
#include "instruction/instruction.h"
#include "symbol/symbol_table_stack.h"
#include "symbol/type.h"
#include "tokenizer/token.h"

namespace miniplc0 {

using std::vector;

class Generator final {
 public:
  Generator() = default;
  void operator+=(Generator &);
  int64_t size() { return _instructions.size(); }

 public:
  void generateOperation(TokenType type, TokenType op);
  void show();
  void generateNega(TokenType type);
  void generateAs(TokenType type, TokenType as);
  void generateLoadVariable(int id, VariableType vt);
  void generateGetVariable(int id, VariableType type);
  void generateDouble(double);
  void generateInt64(int64_t);
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
  SymbolTableStack _symbol_table_stack;
  std::vector<Instruction> _instructions;

 private:
  void generateProgram(ProgNodePtr);

  void generateFunction(FuncNodePtr);
  vector<VariableItem> &generateFunctionParam(FuncNodePtr);

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

  void generateAssignExpr(AssignExprNodePtr);
  void generateUnaryExpr(UnaryExprNodePtr);
  void generateOpExpr(OpExprNode);

  void generateCallExpr(CallExprNodePtr);
};

}  // namespace miniplc0
