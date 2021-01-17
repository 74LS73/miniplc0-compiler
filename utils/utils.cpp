#include "utils.h"

namespace miniplc0 {

uint64_t SwapLong2Endian(const uint64_t &v) {
  uint64_t val = v;
  val = ((val << 8) & 0xFF00FF00FF00FF00) | ((val >> 8) & 0x00FF00FF00FF00FF);
  val = ((val << 16) & 0xFFFF0000FFFF0000) | ((val >> 16) & 0x0000FFFF0000FFFF);
  return (val << 32) | (val >> 32);
}

uint32_t SwapInt2Endian(const uint64_t &v) {
  uint32_t val = uint32_t(v);
  val = ((val << 8) & 0xFF00FF00) | ((val >> 8) & 0x00FF00FF);
  return (val << 16) | (val >> 16);
}

}  // namespace miniplc0
