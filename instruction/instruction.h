#pragma once

#include <cstdint>
#include <utility>

namespace miniplc0 {

enum class Operation { ILL = 0, LIT, LOD, STO, ADD, SUB, MUL, DIV, WRT };
enum ISA {
  NOP = 0x00,
  PUSH,
  POP,
  POPN,
  DUP,
  LOCA = 0x0a,
  ARGA,
  GLOBA,
  LOAD_8 = 0x10,
  LOAD_16,
  LOAD_32,
  LOAD_64,
  STORE_8,
  STORE_32,
  STORE_64,
  ALLOC,
  FREE,
  STACKALLOC,
  ADD_I = 0x20,
  SUB_I,
  MUL_I,
  DIV_I,
  ADD_F,
  SUB_F,
  MUL_F,
  DIV_F,
  DIV_U,
  SHL,
  SHR,
  AND,
  OR,
  XOR,
  NOT,
  CMP_I = 0x30,
  CMP_U,
  CMP_F,
  NEG_I = 0x34,
  NEG_F,
  ITOF,
  FTOI,
  SHRL,
  SET_LT,
  SET_GT,
  BR = 0x41,
  BR_FALSE,
  BR_TRUE,
  CALL = 0x48,
  RET,
  CALLNAME,
  SCANI = 0x50,
  SCANC,
  SCANF,
  PRINTI,
  PRINTC,
  PRINTF,
  PRINTS,
  PRINTLN,
  PANIC = 0xfe,
  BREAK_FAKE,
  CONTINUE_FAKE
};

class Instruction final {
 private:
  using uint64_t = std::uint64_t;

 public:
  friend void swap(Instruction &lhs, Instruction &rhs);

 public:
  Instruction(ISA opr, uint64_t x) : _opr(opr), _x(x), _len(9) {}
  Instruction(ISA opr) : _opr(opr), _x(0), _len(1) {}

  Instruction() : Instruction(ISA::NOP) {}
  Instruction(const Instruction &i) {
    _opr = i._opr;
    _x = i._x;
    _len = i._len;
  }
  Instruction(Instruction &&i) : Instruction() { swap(*this, i); }
  Instruction &operator=(Instruction i) {
    swap(*this, i);
    return *this;
  }
  bool operator==(const Instruction &i) const {
    return _opr == i._opr && _x == i._x;
  }

  ISA GetISA() const { return _opr; }
  uint64_t GetX() const { return _x; }
  void SetISA(ISA isa) { _opr = isa; }
  void SetX(uint64_t x) { _x = x; }

  char *ISA2Hex();
  int64_t GetLen() { return _len; }

 private:
  ISA _opr;
  uint64_t _x;
  int64_t _len;
};

inline void swap(Instruction &lhs, Instruction &rhs) {
  using std::swap;
  swap(lhs._opr, rhs._opr);
  swap(lhs._x, rhs._x);
  swap(lhs._len, rhs._len);
}
}  // namespace miniplc0
