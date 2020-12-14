#include "generator.h"

namespace miniplc0 {

void Generator::operator+=(Generator &rhs) {
  for (auto &ins : rhs._instructions) {
    this->_instructions.emplace_back(ins);
  }
}

void Generator::generateOperation(TokenType type, TokenType op) {
  int bias = type - TokenType::INT;
  switch (op) {
    case TokenType::MULT_SIGN:
      this->_instructions.emplace_back(Instruction(ISA(ISA::MUL_I + bias * 4)));
      break;
    case TokenType::DIV_SIGN:
      this->_instructions.emplace_back(Instruction(ISA(ISA::DIV_I + bias * 4)));
      break;
    case TokenType::PLUS_SIGN:
      this->_instructions.emplace_back(Instruction(ISA(ISA::ADD_I + bias * 4)));
      break;
    case TokenType::MINUS_SIGN:
      this->_instructions.emplace_back(Instruction(ISA(ISA::SUB_I + bias * 4)));
      break;
    case TokenType::EQUAL:
      this->_instructions.emplace_back(Instruction(ISA(ISA::CMP_I + bias * 2)));
      this->_instructions.emplace_back(Instruction(ISA::NOT));
      break;
    case TokenType::NO_EQUAL:
      this->_instructions.emplace_back(Instruction(ISA(ISA::CMP_I + bias * 2)));
      break;
    case TokenType::LESS_SIGN:
      this->_instructions.emplace_back(Instruction(ISA(ISA::CMP_I + bias * 2)));
      this->_instructions.emplace_back(Instruction(ISA::SET_LT));
      break;
    case TokenType::GREATER_SIGN:
      this->_instructions.emplace_back(Instruction(ISA(ISA::CMP_I + bias * 2)));
      this->_instructions.emplace_back(Instruction(ISA::SET_GT));
      break;
    case TokenType::LESS_EQUAL:
      this->_instructions.emplace_back(Instruction(ISA(ISA::CMP_I + bias * 2)));
      this->_instructions.emplace_back(Instruction(ISA::SET_GT));
      this->_instructions.emplace_back(Instruction(ISA::NOT));
      break;
    case TokenType::GREATER_EQUAL:
      this->_instructions.emplace_back(Instruction(ISA(ISA::CMP_I + bias * 2)));
      this->_instructions.emplace_back(Instruction(ISA::SET_LT));
      this->_instructions.emplace_back(Instruction(ISA::NOT));
      break;
    default:
      break;
  }
}

void Generator::generateNega(TokenType type) {
  int bias = type - TokenType::INT;
  this->_instructions.emplace_back(Instruction(ISA(ISA::NEG_I + bias)));
}

void Generator::generateAs(TokenType type, TokenType as) {
  if (type == as) return;
  if (as == TokenType::INT)
    this->_instructions.emplace_back(Instruction(ISA::FTOI));
  else
    this->_instructions.emplace_back(Instruction(ISA::ITOF));
}

void Generator::generateGetVariable(int id, VariableType type) {
  switch (type) {
    case VariableType::LOCAL:
      this->_instructions.emplace_back(Instruction(ISA::LOCA, id));
      break;
    case VariableType::GLOBAL:
      this->_instructions.emplace_back(Instruction(ISA::GLOBA, id));
      break;
    case VariableType::PARAM:
      this->_instructions.emplace_back(Instruction(ISA::ARGA, id));
      break;
  }
}

void Generator::generateLoadVariable(int id, VariableType type) {
  switch (type) {
    case VariableType::LOCAL:
      this->_instructions.emplace_back(Instruction(ISA::LOCA, id));
      break;
    case VariableType::GLOBAL:
      this->_instructions.emplace_back(Instruction(ISA::GLOBA, id));
      break;
    case VariableType::PARAM:
      this->_instructions.emplace_back(Instruction(ISA::ARGA, id));
      break;
  }
  this->_instructions.emplace_back(Instruction(ISA::LOAD_64));
}

void Generator::generateDouble(double val) {
  int64_t *p = reinterpret_cast<int64_t *>(&val);
  this->_instructions.emplace_back(Instruction(ISA::PUSH, *p));
}
void Generator::generateInt64(int64_t val) {
  this->_instructions.emplace_back(Instruction(ISA::PUSH, val));
}

void Generator::generateStackAlloc(int64_t size) {
  this->_instructions.emplace_back(Instruction(ISA::SHRL, size));
}
void Generator::generateCallFunction(int64_t id) {
  this->_instructions.emplace_back(Instruction(ISA::CALL, id));
}

void Generator::generateStore() {
  this->_instructions.emplace_back(Instruction(ISA::STORE_64));
}

void Generator::generateRet() {
  this->_instructions.emplace_back(Instruction(ISA::RET));
}

void Generator::generateBr(int64_t num) {
  this->_instructions.emplace_back(Instruction(ISA::BR, num));
}
void Generator::generateBrTrue(int64_t num) {
  this->_instructions.emplace_back(Instruction(ISA::BR_TRUE, num));
}

void Generator::generateBreak() {
  this->_instructions.emplace_back(Instruction(ISA::BREAK_FAKE));
}

void Generator::generateContinue() {
  this->_instructions.emplace_back(Instruction(ISA::CONTINUE_FAKE));
}

void Generator::fixBreakAndContinue() {
  int pos = 0;
  for (auto &ins : this->_instructions) {
    if (ins.GetISA() == ISA::BREAK_FAKE) {
      ins.SetISA(ISA::BR);
      ins.SetX(this->_instructions.size() - pos - 1);
    }
    else if (ins.GetISA() == ISA::CONTINUE_FAKE) {
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
  for (auto &ins : this->_instructions) {
    std::cout << ISA2Str(ins.GetISA()) << " " << ins.GetX() << std::endl;
  }
}

}  // namespace miniplc0
