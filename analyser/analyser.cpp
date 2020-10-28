#include "analyser.h"

#include <climits>

namespace miniplc0 {

bool isIntegerOverflow(std::string s, int prefix);

std::pair<std::vector<Instruction>, std::optional<CompilationError>>
Analyser::Analyse() {
  auto err = analyseProgram();
  if (err.has_value())
    return std::make_pair(std::vector<Instruction>(), err);
  else
    return std::make_pair(_instructions, std::optional<CompilationError>());
}

/* <程序> ::= 'begin'<主过程>'end' */
std::optional<CompilationError> Analyser::analyseProgram() {
  // 示例函数，示例如何调用子程序

  // 'begin'
  auto bg = nextToken();
  if (!bg.has_value() || bg.value().GetType() != TokenType::BEGIN)
    return std::make_optional<CompilationError>(_current_pos,
                                                ErrorCode::ErrNoBegin);

  // <主过程>
  auto err = analyseMain();
  if (err.has_value()) return err;

  // 'end'
  auto ed = nextToken();
  if (!ed.has_value() || ed.value().GetType() != TokenType::END)
    return std::make_optional<CompilationError>(_current_pos,
                                                ErrorCode::ErrNoEnd);
  return {};
}

/* <主过程> ::= <常量声明><变量声明><语句序列>
   需要补全 
*/
std::optional<CompilationError> Analyser::analyseMain() {
  // 完全可以参照 <程序> 编写
  std::optional<miniplc0::CompilationError> err;
  // <常量声明>
  err = analyseConstantDeclaration();
  if (err.has_value()) return err;

  // <变量声明>
  err = analyseVariableDeclaration();
  if (err.has_value()) return err;
  
  // <语句序列>
  err = analyseStatementSequence();
  if (err.has_value()) return err;

  return {};
}

// <常量声明> ::= {<常量声明语句>}
// <常量声明语句> ::= 'const'<标识符>'='<常表达式>';'
std::optional<CompilationError> Analyser::analyseConstantDeclaration() {
  // 示例函数，示例如何分析常量声明

  // 常量声明语句可能有 0 或无数个
  while (true) {
    // 预读一个 token，不然不知道是否应该用 <常量声明> 推导
    auto next = nextToken();
    if (!next.has_value()) return {};
    // 如果是 const 那么说明应该推导 <常量声明> 否则直接返回
    if (next.value().GetType() != TokenType::CONST) {
      unreadToken();
      return {};
    }

    // <常量声明语句>
    next = nextToken();
    if (!next.has_value() || next.value().GetType() != TokenType::IDENTIFIER)
      return std::make_optional<CompilationError>(_current_pos,
                                                  ErrorCode::ErrNeedIdentifier);
    if (isDeclared(next.value().GetValueString()))
      return std::make_optional<CompilationError>(
          _current_pos, ErrorCode::ErrDuplicateDeclaration);
    addConstant(next.value());

    // '='
    next = nextToken();
    if (!next.has_value() || next.value().GetType() != TokenType::EQUAL_SIGN)
      return std::make_optional<CompilationError>(
          _current_pos, ErrorCode::ErrConstantNeedValue);

    // <常表达式>
    int32_t val;
    auto err = analyseConstantExpression(val);
    if (err.has_value()) return err;

    // ';'
    next = nextToken();
    if (!next.has_value() || next.value().GetType() != TokenType::SEMICOLON)
      return std::make_optional<CompilationError>(_current_pos,
                                                  ErrorCode::ErrNoSemicolon);
    // 生成一次 LIT 指令加载常量
    _instructions.emplace_back(Operation::LIT, val);
  }
  return {};
}

// <变量声明> ::= {<变量声明语句>}
// <变量声明语句> ::= 'var'<标识符>['='<表达式>]';'
// 需要补全
std::optional<CompilationError> Analyser::analyseVariableDeclaration() {
  // 变量声明语句可能有一个或者多个
  std::optional<miniplc0::CompilationError> err;
  while (true) {
    // 预读？
    auto next = nextToken();
    if (!next.has_value()) return {};
    // 'var'
    if (next.value().GetType() != TokenType::VAR) {
      unreadToken();
      return {};
    }

    // <标识符>
    next = nextToken();
    if (!next.has_value() || next.value().GetType() != TokenType::IDENTIFIER)
      return std::make_optional<CompilationError>(_current_pos,
                                                  ErrorCode::ErrNeedIdentifier);  
    
    auto ident =
      /*标识符的 token*/ Token(next.value());

    if (isDeclared(next.value().GetValueString()))
      return std::make_optional<CompilationError>(
          _current_pos, ErrorCode::ErrDuplicateDeclaration);


    // 变量可能没有初始化，仍然需要一次预读
    bool initialized = /*填写*/ false;
    
    // '='
    next = nextToken();
    if (next.has_value() && next.value().GetType() == TokenType::EQUAL_SIGN){
      // '<表达式>'
      err = analyseExpression();
      if (err.has_value()) return err;
      initialized = true;
    }
    else {
      unreadToken();
    }
    
    // ';'
    next = nextToken();
    if (!next.has_value() || next.value().GetType() != TokenType::SEMICOLON)
      return std::make_optional<CompilationError>(_current_pos,
                                                  ErrorCode::ErrNoSemicolon);
   
    
    if (initialized) {
      addVariable(ident);
      // 已经初始化的变量的值的位置正好是之前表达式计算结果，所以不做处理
    } else {
      addUninitializedVariable(ident);
      // 加载一个任意的初始值
      _instructions.emplace_back(Operation::LIT, 0);
    }
  }
  return {};
}

/* <语句序列> ::= {<语句>}
   <语句> :: = <赋值语句> | <输出语句> | <空语句>
   <赋值语句> :: = <标识符>'='<表达式>';'
   <输出语句> :: = 'print' '(' <表达式> ')' ';'
   <空语句> :: = ';' 
   需要补全 */
std::optional<CompilationError> Analyser::analyseStatementSequence() {
  while (true) {
    // 预读
    auto next = nextToken();
    if (!next.has_value()) return {};
    // unreadToken();
    if (next.value().GetType() != TokenType::IDENTIFIER &&
        next.value().GetType() != TokenType::PRINT &&
        next.value().GetType() != TokenType::SEMICOLON) {
      unreadToken();
      return {};
    }
    std::optional<CompilationError> err;
    switch (next.value().GetType()) {
      case TokenType::IDENTIFIER: {
        unreadToken();
        err = analyseAssignmentStatement();
        break;
      }
      case TokenType::PRINT: {
        unreadToken();
        err = analyseOutputStatement();
        break;
      }
      case TokenType::SEMICOLON: {
        break;
      }
      // 这里需要你针对不同的预读结果来调用不同的子程序
      // 注意我们没有针对空语句单独声明一个函数，因此可以直接在这里返回
      default:
        break;
    }
    if (err.has_value()) return err;
  }
  return {};
}

/* <常表达式> ::= [<符号>]<无符号整数>
   需要补全
*/
std::optional<CompilationError> Analyser::analyseConstantExpression(
    int32_t &out) {
  
  std::optional<CompilationError> err;
  auto next = nextToken();
  int prefix = 1;
  if (!next.has_value()) 
    return std::make_optional<CompilationError>(_current_pos,
                                                ErrorCode::ErrConstantNeedValue);
  else if (next.value().GetType() == TokenType::PLUS_SIGN) {
    prefix = 1;
    next = nextToken();
  }
  else if (next.value().GetType() == TokenType::MINUS_SIGN) {
    prefix = -1;
    next = nextToken();
  }
  
  if (next.value().GetType() != TokenType::UNSIGNED_INTEGER) {
    return std::make_optional<CompilationError>(_current_pos,
                                                ErrorCode::ErrInvalidAssignment);
  }

  // TODO
  // out 是常表达式的结果
  // 这里你要分析常表达式并且计算结果
  // 注意以下均为常表达式
  // +1 -1 1
  // 同时要注意是否溢出
  std::string num = std::any_cast<std::string> (next.value().GetValue());

  if (isIntegerOverflow(num, prefix)){
    return std::make_optional<CompilationError>(_current_pos,
                                                ErrorCode::ErrIntegerOverflow);
  }

  out = prefix * atoi(num.c_str());

  return {};
}

// <表达式> ::= <项>{<加法型运算符><项>}
std::optional<CompilationError> Analyser::analyseExpression() {
  // <项>
  auto err = analyseItem();
  if (err.has_value()) return err;

  // {<加法型运算符><项>}
  while (true) {
    // 预读
    auto next = nextToken();
    if (!next.has_value()) return {};
    auto type = next.value().GetType();
    if (type != TokenType::PLUS_SIGN && type != TokenType::MINUS_SIGN) {
      unreadToken();
      return {};
    }

    // <项>
    err = analyseItem();
    if (err.has_value()) return err;

    // 根据结果生成指令
    if (type == TokenType::PLUS_SIGN)
      _instructions.emplace_back(Operation::ADD, 0);
    else if (type == TokenType::MINUS_SIGN)
      _instructions.emplace_back(Operation::SUB, 0);
  }
  return {};
}

// <赋值语句> ::= <标识符>'='<表达式>';'
// 需要补全
std::optional<CompilationError> Analyser::analyseAssignmentStatement() {
  // 这里除了语法分析以外还要留意
  std::optional<miniplc0::CompilationError> err;
  auto next = nextToken();
  if (!next.has_value() || next.value().GetType() != TokenType::IDENTIFIER) {
    return std::make_optional<CompilationError>(_current_pos,
                                                ErrorCode::ErrNeedIdentifier);
  }
  std::string ident = std::any_cast<std::string> (next.value().GetValue());
  // 标识符声明过吗？
  if (!isDeclared(ident)) {
    return std::make_optional<CompilationError>(_current_pos,
                                            ErrorCode::ErrNotDeclared);
  }
  // 标识符是常量吗？
  if (isConstant(ident)) {
    return std::make_optional<CompilationError>(_current_pos,
                                            ErrorCode::ErrAssignToConstant);
  }

    // 存储这个标识符
  int32_t index = getIndex(ident);
  _instructions.emplace_back(Operation::STO, index);
  if (!isInitializedVariable(ident)) makeInitialized(ident);

  // TODO
  next = nextToken();
  if (next.value().GetType() != TokenType::EQUAL_SIGN) {
        return std::make_optional<CompilationError>(_current_pos,
                                            ErrorCode::ErrInvalidAssignment);
  }

  err = analyseExpression();
  if (err.has_value()) return err;

  next = nextToken();
  if (next.value().GetType() != TokenType::SEMICOLON) {
        return std::make_optional<CompilationError>(_current_pos,
                                            ErrorCode::ErrNoSemicolon);
  }

  return {};
}

// <输出语句> ::= 'print' '(' <表达式> ')' ';'
std::optional<CompilationError> Analyser::analyseOutputStatement() {
  // 如果之前 <语句序列> 的实现正确，这里第一个 next 一定是 TokenType::PRINT
  auto next = nextToken();

  next = nextToken();
  if (!next.has_value() || next.value().GetType() != TokenType::LEFT_BRACKET)
    return std::make_optional<CompilationError>(_current_pos,
                                                ErrorCode::ErrInvalidPrint);

  // <表达式>
  auto err = analyseExpression();
  if (err.has_value()) return err;

  // ')'
  next = nextToken();
  if (!next.has_value() || next.value().GetType() != TokenType::RIGHT_BRACKET)
    return std::make_optional<CompilationError>(_current_pos,
                                                ErrorCode::ErrInvalidPrint);

  // ';'
  next = nextToken();
  if (!next.has_value() || next.value().GetType() != TokenType::SEMICOLON)
    return std::make_optional<CompilationError>(_current_pos,
                                                ErrorCode::ErrNoSemicolon);

  // 生成相应的指令 WRT
  _instructions.emplace_back(Operation::WRT, 0);
  return {};
}

// <项> :: = <因子>{ <乘法型运算符><因子> }
// 需要补全
std::optional<CompilationError> Analyser::analyseItem() {
  // 可以参考 <表达式> 实现  
  std::optional<miniplc0::CompilationError> err;
  std::optional<miniplc0::Token> next;

  // <因子>
  err = analyseFactor();
  if (err.has_value()) return err;

  // { <乘法型运算符><因子> }
  while (true) {
    // 预读
    next = nextToken();

    if (!next.has_value()) break;
    
    auto type = next.value().GetType();

    if (type != TokenType::MULTIPLICATION_SIGN && type != TokenType::DIVISION_SIGN) {
      unreadToken();
      return {};
    }

    // <因子>
    err = analyseFactor();
    if (err.has_value()) return err;

    // 根据结果生成指令
    if (type == TokenType::MULTIPLICATION_SIGN)
      _instructions.emplace_back(Operation::MUL, 0);
    else if (type == TokenType::DIVISION_SIGN)
      _instructions.emplace_back(Operation::DIV, 0);
  }
  return {};
}

// <因子> ::= [<符号>]( <标识符> | <无符号整数> | '('<表达式>')' )
// 需要补全
std::optional<CompilationError> Analyser::analyseFactor() {
  // [<符号>]
  auto next = nextToken();
  auto prefix = 1;
  if (!next.has_value())
    return std::make_optional<CompilationError>(
        _current_pos, ErrorCode::ErrIncompleteExpression);
  if (next.value().GetType() == TokenType::PLUS_SIGN)
    prefix = 1;
  else if (next.value().GetType() == TokenType::MINUS_SIGN) {
    prefix = -1;
    _instructions.emplace_back(Operation::LIT, 0);
  } else
    unreadToken();

  std::optional<miniplc0::CompilationError> err;

  // 预读
  next = nextToken();
  if (!next.has_value())
    return std::make_optional<CompilationError>(
        _current_pos, ErrorCode::ErrIncompleteExpression);
  switch (next.value().GetType()) {
    case TokenType::IDENTIFIER: {
      // - 加载变量
      auto ident = std::any_cast<std::string>(next.value().GetValue());
      if (!isDeclared(ident))
        return {CompilationError(_current_pos, ErrorCode::ErrNotDeclared)};
      if (!isInitializedVariable(ident) && !isConstant(ident))
        return {CompilationError(_current_pos,
        ErrorCode::ErrNotInitialized)};
      _instructions.emplace_back(Operation::LOD, getIndex(ident));
      break;
    }
    case TokenType::UNSIGNED_INTEGER: {
      // TODO
      
      std::string num = std::any_cast<std::string> (next.value().GetValue());

      if (isIntegerOverflow(num, prefix)){
        return std::make_optional<CompilationError>(_current_pos,
                                                    ErrorCode::ErrIntegerOverflow);
      }
      int val = atoi(num.c_str());
      _instructions.emplace_back(Operation::LIT, val);
      break;
    }
    case TokenType::LEFT_BRACKET: {
      err = analyseExpression();
      if (!next.has_value()) return err;

      next = nextToken();
      if (next.value().GetType() != TokenType::RIGHT_BRACKET) {
        return std::make_optional<CompilationError>(
        _current_pos, ErrorCode::ErrIncompleteExpression);
      }
      break;
    }
    default:
      return std::make_optional<CompilationError>(
          _current_pos, ErrorCode::ErrIncompleteExpression);
  }

  // 取负
  if (prefix == -1) _instructions.emplace_back(Operation::SUB, 0);
  return {};
}

std::optional<Token> Analyser::nextToken() {
  if (_offset == _tokens.size()) return {};
  // 考虑到 _tokens[0..._offset-1] 已经被分析过了
  // 所以我们选择 _tokens[0..._offset-1] 的 EndPos 作为当前位置
  _current_pos = _tokens[_offset].GetEndPos();
  return _tokens[_offset++];
}

void Analyser::unreadToken() {
  if (_offset == 0) DieAndPrint("analyser unreads token from the begining.");
  _current_pos = _tokens[_offset - 1].GetEndPos();
  _offset--;
}

void Analyser::_add(const Token &tk, std::map<std::string, int32_t> &mp) {
  if (tk.GetType() != TokenType::IDENTIFIER)
    DieAndPrint("only identifier can be added to the table.");
  mp[tk.GetValueString()] = _nextTokenIndex;
  _nextTokenIndex++;
}

void Analyser::addVariable(const Token &tk) { _add(tk, _vars); }

void Analyser::addConstant(const Token &tk) { _add(tk, _consts); }

void Analyser::addUninitializedVariable(const Token &tk) {
  _add(tk, _uninitialized_vars);
}

void Analyser::makeInitialized(const std::string &var_name) {
  auto var = _uninitialized_vars.find(var_name);
  if (var == _uninitialized_vars.end())
    DieAndPrint("Variable not found in uninitialized area. bad bad");
  auto item = _uninitialized_vars.extract(var);
  _vars.insert(std::move(item));
}

int32_t Analyser::getIndex(const std::string &s) {
  if (_uninitialized_vars.find(s) != _uninitialized_vars.end())
    return _uninitialized_vars[s];
  else if (_vars.find(s) != _vars.end())
    return _vars[s];
  else
    return _consts[s];
}

bool Analyser::isDeclared(const std::string &s) {
  return isConstant(s) || isUninitializedVariable(s) ||
         isInitializedVariable(s);
}

bool Analyser::isUninitializedVariable(const std::string &s) {
  return _uninitialized_vars.find(s) != _uninitialized_vars.end();
}
bool Analyser::isInitializedVariable(const std::string &s) {
  return _vars.find(s) != _vars.end();
}

bool Analyser::isConstant(const std::string &s) {
  return _consts.find(s) != _consts.end();
}

bool isIntegerOverflow(std::string s, int prefix) {
  if (s.length() < 10) return false;
  else if (s.length() > 10) return true;
  else {
    if (s > "2147483648") return true;
    else if (s == "2147483648") return prefix != -1;
    else return false;
  }
}

}  // namespace miniplc0
