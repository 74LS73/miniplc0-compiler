#include "instruction.h"

#include<cstdlib>

namespace miniplc0 {

uint64_t swap_endian(uint64_t &val) {
  val = ((val << 8) & 0xFF00FF00FF00FF00) | ((val >> 8) & 0x00FF00FF00FF00FF);
  val = ((val << 16) & 0xFFFF0000FFFF0000) | ((val >> 16) & 0x0000FFFF0000FFFF);
  return (val << 32) | (val >> 32);
}


char *Instruction::ISA2Hex() {
  
  if (_len == 1) {
    char *val = (char *) malloc(sizeof(char));
    val[0] = _opr;
    return val;
  }
  
  char *val = (char *) malloc(sizeof(char) * 9);
  val[0] = _opr;
  uint64_t *p = (uint64_t *)(val + 1);
  *p = swap_endian(_x);
  return val;
}

}  // namespace miniplc0
