#include "instruction.h"

#include <cstdlib>

#include "utils/utils.h"

namespace miniplc0 {

char *Instruction::ISA2Hex() {
  char *val;
  if (_len == 1) {
    val = (char *)malloc(sizeof(char));
    val[0] = _opr;
    return val;
  } else if (_len == 5) {
    val = (char *)malloc(sizeof(char) * 5);
    val[0] = _opr;
    uint32_t *p = (uint32_t *)(val + 1);
    *p = SwapInt2Endian(_x);
  } else {
    val = (char *)malloc(sizeof(char) * 9);
    val[0] = _opr;
    uint64_t *p = (uint64_t *)(val + 1);
    *p = SwapLong2Endian(_x);
  }
  return val;
}

}  // namespace miniplc0
