#include "generator.h"

#include "utils/utils.h"
namespace miniplc0 {

// void Generator::operator+=(Generator &rhs) {
//   for (auto &ins : rhs._cur_block) {
//     _cur_block.emplace_back(ins);
//   }
// }

void Generator::generateOperation(TokenType type, TokenType op) {
  auto &_cur_block = _code_stack.top();
  int bias = type - TokenType::INT;
  switch (op) {
    case TokenType::MULT_SIGN:
      _cur_block.emplace_back(Instruction(ISA(ISA::MUL_I + bias * 4)));
      break;
    case TokenType::DIV_SIGN:
      _cur_block.emplace_back(Instruction(ISA(ISA::DIV_I + bias * 4)));
      break;
    case TokenType::PLUS_SIGN:
      _cur_block.emplace_back(Instruction(ISA(ISA::ADD_I + bias * 4)));
      break;
    case TokenType::MINUS_SIGN:
      _cur_block.emplace_back(Instruction(ISA(ISA::SUB_I + bias * 4)));
      break;
    case TokenType::EQUAL:
      _cur_block.emplace_back(Instruction(ISA(ISA::CMP_I + bias * 2)));
      _cur_block.emplace_back(Instruction(ISA::NOT));
      break;
    case TokenType::NO_EQUAL:
      _cur_block.emplace_back(Instruction(ISA(ISA::CMP_I + bias * 2)));
      break;
    case TokenType::LESS_SIGN:
      _cur_block.emplace_back(Instruction(ISA(ISA::CMP_I + bias * 2)));
      _cur_block.emplace_back(Instruction(ISA::SET_LT));
      break;
    case TokenType::GREATER_SIGN:
      _cur_block.emplace_back(Instruction(ISA(ISA::CMP_I + bias * 2)));
      _cur_block.emplace_back(Instruction(ISA::SET_GT));
      break;
    case TokenType::LESS_EQUAL:
      _cur_block.emplace_back(Instruction(ISA(ISA::CMP_I + bias * 2)));
      _cur_block.emplace_back(Instruction(ISA::SET_GT));
      _cur_block.emplace_back(Instruction(ISA::NOT));
      break;
    case TokenType::GREATER_EQUAL:
      _cur_block.emplace_back(Instruction(ISA(ISA::CMP_I + bias * 2)));
      _cur_block.emplace_back(Instruction(ISA::SET_LT));
      _cur_block.emplace_back(Instruction(ISA::NOT));
      break;
    default:
      break;
  }
}

void Generator::generateNega(TokenType type) {
  auto &_cur_block = _code_stack.top();
  int bias = type - TokenType::INT;
  _cur_block.emplace_back(Instruction(ISA(ISA::NEG_I + bias)));
}

void Generator::generateAs(TokenType type, TokenType as) {
  auto &_cur_block = _code_stack.top();
  if (type == as) return;
  if (as == TokenType::INT)
    _cur_block.emplace_back(Instruction(ISA::FTOI));
  else
    _cur_block.emplace_back(Instruction(ISA::ITOF));
}

void Generator::generateGetVariable(int id, VariableType type) {
  auto &_cur_block = _code_stack.top();
  switch (type) {
    case VariableType::LOCAL:
      _cur_block.emplace_back(Instruction(ISA::LOCA, id));
      break;
    case VariableType::GLOBAL:
      _cur_block.emplace_back(Instruction(ISA::GLOBA, id));
      break;
    case VariableType::PARAM:
      _cur_block.emplace_back(Instruction(ISA::ARGA, id + _cur_func->_return_slots));
      break;
  }
}

void Generator::generateLoadVariable(int id, VariableType type) {
  auto &_cur_block = _code_stack.top();
  switch (type) {
    case VariableType::LOCAL:
      _cur_block.emplace_back(Instruction(ISA::LOCA, id));
      break;
    case VariableType::GLOBAL:
      _cur_block.emplace_back(Instruction(ISA::GLOBA, id));
      break;
    case VariableType::PARAM:
      _cur_block.emplace_back(Instruction(ISA::ARGA, id + _cur_func->_return_slots));
      break;
  }
  _cur_block.emplace_back(Instruction(ISA::LOAD_64));
}

void Generator::generateLiteralValue(int64_t *val) {
  auto &_cur_block = _code_stack.top();
  _cur_block.emplace_back(Instruction(ISA::PUSH, *val));
}

void Generator::generateStackAlloc(int64_t size) {
  auto &_cur_block = _code_stack.top();
  _cur_block.emplace_back(Instruction(ISA::STACKALLOC, size));
}
void Generator::generateCallFunction(int64_t id) {
  auto &_cur_block = _code_stack.top();
  _cur_block.emplace_back(Instruction(ISA::CALL, id));
}

void Generator::generateCallName(int64_t global_index) {
  auto &_cur_block = _code_stack.top();
  _cur_block.emplace_back(Instruction(ISA::CALLNAME, global_index));
}

void Generator::generateStore() {
  auto &_cur_block = _code_stack.top();
  _cur_block.emplace_back(Instruction(ISA::STORE_64));
}

void Generator::generateRet() {
  auto &_cur_block = _code_stack.top();
  _cur_block.emplace_back(Instruction(ISA::RET));
}

void Generator::generateBr(int64_t num) {
  auto &_cur_block = _code_stack.top();
  _cur_block.emplace_back(Instruction(ISA::BR, num));
}
void Generator::generateBrTrue(int64_t num) {
  auto &_cur_block = _code_stack.top();
  _cur_block.emplace_back(Instruction(ISA::BR_TRUE, num));
}

void Generator::generateBreak() {
  auto &_cur_block = _code_stack.top();
  _cur_block.emplace_back(Instruction(ISA::BREAK_FAKE));
}

void Generator::generateContinue() {
  auto &_cur_block = _code_stack.top();
  _cur_block.emplace_back(Instruction(ISA::CONTINUE_FAKE));
}

void Generator::fixBreakAndContinue() {
  auto &_cur_block = _code_stack.top();
  int pos = 0;
  for (auto &ins : _cur_block) {
    if (ins.GetISA() == ISA::BREAK_FAKE) {
      ins.SetISA(ISA::BR);
      ins.SetX(_cur_block.size() - pos - 1);
    } else if (ins.GetISA() == ISA::CONTINUE_FAKE) {
      ins.SetISA(ISA::BR);
      ins.SetX(-pos);
    }
    pos++;
  }
}

std::string ISA2Str(ISA isa) {
  switch (isa) {
    case NOP:
      return "NOP";
    case PUSH:
      return "PUSH";
    case POP:
      return "POP";
    case POPN:
      return "POPN";
    case DUP:
      return "DUP";
    case LOCA:
      return "LOCA";
    case ARGA:
      return "ARGA";
    case GLOBA:
      return "GLOBA";
    case LOAD_64:
      return "LOAD_64";
    case STORE_64:
      return "STORE_64";
    case ALLOC:
      return "ALLOC";
    case FREE:
      return "FREE";
    case STACKALLOC:
      return "STACKALLOC";
    case ADD_I:
      return "ADD_I";
    case SUB_I:
      return "SUB_I";
    case MUL_I:
      return "MUL_I";
    case DIV_I:
      return "DIV_I";
    case ADD_F:
      return "ADD_F";
    case SUB_F:
      return "SUB_F";
    case MUL_F:
      return "MUL_F";
    case DIV_F:
      return "DIV_F";
    case DIV_U:
      return "DIV_U";
    case SHL:
      return "SHL";
    case SHR:
      return "SHR";
    case AND:
      return "AND";
    case OR:
      return "OR";
    case XOR:
      return "XOR";
    case NOT:
      return "NOT";
    case CMP_I:
      return "CMP_I";
    case CMP_U:
      return "CMP_U";
    case CMP_F:
      return "CMP_F";
    case NEG_I:
      return "NEG_I";
    case NEG_F:
      return "NEG_F";
    case ITOF:
      return "ITOF";
    case FTOI:
      return "FTOI";
    case SHRL:
      return "SHRL";
    case SET_LT:
      return "SET_LT";
    case SET_GT:
      return "SET_GT";
    case BR:
      return "BR";
    case BR_FALSE:
      return "BR_FALSE";
    case BR_TRUE:
      return "BR_TRUE";
    case CALL:
      return "CALL";
    case RET:
      return "RET";
    case CALLNAME:
      return "CALLNAME";
    case SCANI:
      return "SCANI";
    case SCANC:
      return "SCANC";
    case SCANF:
      return "SCANF";
    case PRINTI:
      return "PRINTI";
    case PRINTC:
      return "PRINTC";
    case PRINTF:
      return "PRINTF";
    case PRINTS:
      return "PRINTS";
    case PRINTLN:
      return "PRINTLN";
    case PANIC:
      return "PANIC";
    default:
      return "error";
  }
}

void Generator::show() {
  auto &_cur_block = _code_stack.top();
  for (auto &ins : _cur_block) {
    std::cout << ISA2Str(ins.GetISA()) << " " << ins.GetX() << std::endl;
  }
}

void Generator::_Write(const uint64_t &v, size_t __n) {
  if (__n == 1) {
    uint8_t val = uint8_t(v);
    _output->write((char *)(&val), 1);
  } else if (__n == 4) {
    uint32_t val = SwapInt2Endian(v);
    _output->write((char *)(&val), 4);
  } else {
    uint64_t val = SwapLong2Endian(v);
    _output->write((char *)(&val), 8);
  }
}

}  // namespace miniplc0
