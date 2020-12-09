#include "generator.h"

namespace miniplc0 {

void Generator::operator+=(Generator &rhs) {
  for (auto &ins : rhs._instructions) {
    this->_instructions.emplace_back(ins);
  }
}

void Generator::addOperation(TokenType type, TokenType op) {
  int bias = type - TokenType::INT;
  switch (op) {
    case TokenType::MULT_SIGN:
      this->_instructions.emplace_back(
          Instruction(ISA(ISA::MUL_I + bias * 4), 0));
      break;
    case TokenType::DIV_SIGN:
      this->_instructions.emplace_back(
          Instruction(ISA(ISA::DIV_I + bias * 4), 0));
      break;
    case TokenType::PLUS_SIGN:
      this->_instructions.emplace_back(
          Instruction(ISA(ISA::ADD_I + bias * 4), 0));
      break;
    case TokenType::MINUS_SIGN:
      this->_instructions.emplace_back(
          Instruction(ISA(ISA::SUB_I + bias * 4), 0));
      break;
    case TokenType::EQUAL:
      this->_instructions.emplace_back(
          Instruction(ISA(ISA::CMP_I + bias * 2), 0));
      break;
    case TokenType::NO_EQUAL:
      this->_instructions.emplace_back(Instruction(ISA::NOT, 0));
      break;
    case TokenType::LESS_SIGN:
      this->_instructions.emplace_back(
          Instruction(ISA(ISA::CMP_I + bias * 2), 0));
      break;
    case TokenType::GREATER_SIGN:
      this->_instructions.emplace_back(
          Instruction(ISA(ISA::CMP_I + bias * 2), 0));
      break;
    case TokenType::LESS_EQUAL:
      this->_instructions.emplace_back(
          Instruction(ISA(ISA::CMP_I + bias * 2), 0));
      break;
    case TokenType::GREATER_EQUAL:
      this->_instructions.emplace_back(
          Instruction(ISA(ISA::CMP_I + bias * 2), 0));
      break;
    default:
      break;
  }
}

void Generator::generateNega(TokenType type) {
  int bias = type - TokenType::INT;
  this->_instructions.emplace_back(Instruction(ISA(ISA::NEG_I + bias), 0));
}

void Generator::generateAs(TokenType type, TokenType as) {
  if (type == as) return;
  if (as == TokenType::INT)
    this->_instructions.emplace_back(Instruction(ISA::FTOI, 0));
  else
    this->_instructions.emplace_back(Instruction(ISA::ITOF, 0));
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
}

void Generator::generateDouble(double val) {
  int64_t *p = reinterpret_cast<int64_t *> (&val);
  this->_instructions.emplace_back(Instruction(ISA::PUSH, *p));
}
void Generator::generateInt64(int64_t val) {
  this->_instructions.emplace_back(Instruction(ISA::PUSH, val));
}

void Generator::show() {
  for (auto &ins : this->_instructions) {
    std::cout << ins.GetISA()<<" "<< ins.GetX() << std::endl;
  }
}

}  // namespace miniplc0
