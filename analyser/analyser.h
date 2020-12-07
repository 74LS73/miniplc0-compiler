#pragma once

#include <cstddef>  // for std::size_t
#include <cstdint>
#include <map>
#include <optional>
#include <utility>
#include <vector>

#include "error/error.h"
#include "instruction/instruction.h"
#include "item.h"
#include "symbol.h"
#include "tokenizer/token.h"

namespace miniplc0 {

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
        _current_pos(0, 0),
        _uninitialized_vars({}),
        _vars({}),
        _consts({}),
        _nextTokenIndex(0) {}
  Analyser(Analyser &&) = delete;
  Analyser(const Analyser &) = delete;
  Analyser &operator=(Analyser) = delete;

  // 唯一接口
  std::pair<std::vector<Instruction>, std::optional<CompilationError>>
  Analyse();

 private:
  // 所有的递归子程序
  // <常表达式>
  // 这里的 out 是常表达式的值
  std::optional<CompilationError> analyseConstantExpression(int32_t &out);

  /* 以下为表达式 */

  // <表达式>
  std::optional<CompilationError> analyseExpression();
  // <项表达式>
  std::optional<CompilationError> analyserItemExpression(OUT OperatorItem &);
  // <一元运算符表达式>
  std::optional<CompilationError> analyserUnaryExpression(OUT OperatorItem &);
  // <运算符表达式>
  std::optional<CompilationError> analyseOperatorExpression(
      IN OperatorItem &, IN TokenType = TokenType::NULL_TOKEN);
  // <取反表达式> negate_expr -> '-' expr
  std::optional<CompilationError> analyseNegateExpression();
  // <赋值表达式>assign_expr -> l_expr '=' expr
  std::optional<CompilationError> analyseAssignExpression();
  // <类型转换表达式> as_expr -> expr 'as' ty
  std::optional<CompilationError> analyseAsExpression();
  // <函数调用表达式>
  std::optional<CompilationError> analyseCallExpression();
  // <字面量表达式>
  std::optional<CompilationError> analyseLiteralExpression();
  // <标识符表达式>
  std::optional<CompilationError> analyseIdentExpression();
  // <括号表达式>
  std::optional<CompilationError> analyseBracketExpression();

  /* 以下为语句 */
  // <语句>
  std::optional<CompilationError> analyseStatement(FunctionItem &);
  // <表达式语句>
  std::optional<CompilationError> analyseExprStatement(FunctionItem &);
  // <变量声明语句>
  std::optional<CompilationError> analyseDeclVariableStatement(FunctionItem &);
  // <常量声明语句>
  std::optional<CompilationError> analyseDeclConstStatement(FunctionItem &);
  // <if 语句>
  std::optional<CompilationError> analyseIfStatement(FunctionItem &);
  // <while 语句>
  std::optional<CompilationError> analyseWhileStatement(FunctionItem &);
  // <return 语句>
  std::optional<CompilationError> analyseReturnStatement(FunctionItem &);
  // <代码块>
  std::optional<CompilationError> analyseBlockStatement(FunctionItem &);
  // <空语句>
  std::optional<CompilationError> analyseEmptyStatement();

  /* 以下为函数 */
  // 函数
  std::optional<CompilationError> analyseFunction();
  // 参数
  std::optional<CompilationError> analyseFunctionParameter(FunctionItem &);
  // 参数列表
  std::optional<CompilationError> analyseFunctionParamList(FunctionItem &);

  /* 以下为程序结构 */
  std::optional<CompilationError> analyseProgram();

 private:
  /* 一些简单工具函数 */
  bool isIntegerOverflow(std::string s, int prefix);
  // Token 缓冲区相关操作

  // 返回下一个 token
  std::optional<Token> nextToken();
  // 回退一个 token
  void unreadToken();

  /* 下面是符号表相关操作 */
  void _initTableStack();
  // 是否是常量
  bool isConstant(const std::string &);
  // 获得 {变量，常量} 在栈上的偏移
  int32_t getIndex(const std::string &);

  void pushNextScope();
  void popCurrentScope();
  Symbol getCurrentTable();
  int getCurrentScopeLevel();

  // 是否被声明过
  bool isLocalVariableDeclared(const std::string &);
  bool isFunctionDeclared(const std::string &);
  bool isGlobalVariableDeclared(const std::string &);

  void declareVariable(const Token &tk, VariableItem &);
  void declareFunction(const Token &tk, FunctionItem &);

 private:
  std::vector<Symbol> _table_stack;
  // 当前作用域
  int _cur_scope_level = 0;
  const int _global_scope_level = 0;
  std::vector<Token> _tokens;
  std::size_t _offset;
  std::vector<Instruction> _instructions;
  std::pair<uint64_t, uint64_t> _current_pos;

  // 为了简单处理，我们直接把符号表耦合在语法分析里
  // 变量                   示例
  // _uninitialized_vars    var a;
  // _vars                  var a=1;
  // _consts                const a=1;
  std::map<std::string, int32_t> _uninitialized_vars;
  std::map<std::string, int32_t> _vars;
  std::map<std::string, int32_t> _consts;
  // 下一个 token 在栈的偏移
  int32_t _nextTokenIndex;
};

}  // namespace miniplc0
