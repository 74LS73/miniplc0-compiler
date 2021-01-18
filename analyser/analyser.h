#pragma once

#include <cstddef>  // for std::size_t
#include <cstdint>
#include <map>
#include <optional>
#include <utility>
#include <vector>

#include "ast/ast.h"
#include "error/error.h"
#include "tokenizer/token.h"
#include "utils.h"
#include "symbol/symbol_table_stack.h"

namespace miniplc0 {

using std::optional;
using std::pair;
using std::shared_ptr;
using std::vector;
typedef CompilationError AnalyserError;

#define IN
#define OUT

class Analyser final {
 private:
  using uint64_t = std::uint64_t;
  using int64_t = std::int64_t;
  using uint32_t = std::uint32_t;
  using int32_t = std::int32_t;

 public:
  Analyser(std::vector<Token> v)
      : _tokens(std::move(v)),
        _offset(0),
        _instructions({}),
        _current_pos(0, 0) {}
  Analyser(Analyser &&) = delete;
  Analyser(const Analyser &) = delete;
  Analyser &operator=(Analyser) = delete;

  // 唯一接口
  ProgNodePtr Analyse();

 private:
  // 所有的递归子程序

  /* 以下为表达式 */

  // <表达式>
  ExprNodePtr analyseExpression();
  // <项表达式>
  ExprNodePtr analyserItemExpression();
  // <一元运算符表达式>
  ExprNodePtr analyserUnaryExpression();
  // <运算符表达式>
  ExprNodePtr analyseOperatorExpression(OpExprNodePtr);
  // <赋值表达式>assign_expr -> l_expr '=' expr
  ExprNodePtr analyseAssignExpression();
  // <函数调用表达式>
  ExprNodePtr analyseCallExpression();
  // <字面量表达式>
  ExprNodePtr analyseLiteralExpression();
  // <标识符表达式>
  IdentExprNodePtr analyseIdentExpression();
  // <括号表达式>
  ExprNodePtr analyseBracketExpression();

  /* 以下为语句 */
  // <语句>
  StatNodePtr analyseStatement();
  // <表达式语句>
  StatNodePtr analyseExprStatement();
  // <声明语句>
  DeclStatNodePtr analyseDeclStatement(VariableType);
  // <if 语句>
  StatNodePtr analyseIfStatement();
  // <while 语句>
  StatNodePtr analyseWhileStatement();
  // <break 语句>
  StatNodePtr analyseBreakStatement();
  // <continue 语句>
  StatNodePtr analyseContinueStatement();
  // <return 语句>
  StatNodePtr analyseReturnStatement();
  // <代码块>
  BlockStatNodePtr analyseBlockStatement();
  // <空语句>
  StatNodePtr analyseEmptyStatement();

  /* 以下为函数 */
  // 函数
  FuncNodePtr analyseFunction();
  // 参数
  void analyseFunctionParameter(FuncNodePtr &);
  // 参数列表
  void analyseFunctionParamList(FuncNodePtr &);

  /* 以下为程序结构 */
  ProgNodePtr analyseProgram();

 private:
  // 返回下一个 token
  optional<Token> nextToken();
  optional<TokenType> peekATokenGetType();
  // 回退一个 token
  void unreadToken();

 private:
  SymbolTableStack _symbol_table_stack;
  std::vector<Token> _tokens;
  std::size_t _offset;
  std::vector<Instruction> _instructions;
  std::pair<uint64_t, uint64_t> _current_pos;
  bool _is_in_while = false;
};

}  // namespace miniplc0
