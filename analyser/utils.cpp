#include "utils.h"

#include <cmath>

namespace miniplc0 {

int64_t StringToUnsignedInt64(std::string s) {
  __int128_t res = 0;
  //int64_t res = 0;
  for (char &c : s) {
    res += c - '0';
    res *= 10;
  }
  res /= 10;
  
  if (res > __UINT64_MAX__) return -1;
  return res;
}

double StringToUnsignedDouble(std::string s) {
  __int128_t before_point = 0;
  int i = 0;
  int len = s.size();
  while (i < len && s[i] != '.') {
    before_point += s[i] - '0';
    before_point *= 10;
    ++i;
  }
  before_point/=10;
  ++i;

  double after_point = 0;
  __int128_t base = 10;
  while (i < len && (s[i] != 'e' && s[i] != 'E')) {
    after_point += s[i] - '0';
    after_point *= 10;
    base *=10;
    ++i;
  }
  after_point /= base;
  ++i;

  int prefix = 1;
  if (i<len && s[i]=='+') {
    ++i;
  }else if (i<len && s[i]=='-') {
    ++i;
    prefix = - prefix;
  }
  
  base = 0;
  while (i < len) {
    base += s[i] - '0';
    base *= 10;
    ++i;
  }
  base /= 10;
  return (before_point + after_point) * std::pow(10, prefix * base);
}

std::string RemoveIntPrefixZero(std::string s) {
  int pos = s.find_first_not_of('0');
  return s.substr(pos, -1);
}

bool IsSignedInt64Overflow(std::string s, int prefix) {
  if (s.length() < 20)
    return false;
  else if (s.length() >= 20)
    return true;
  else {
    if (s > "9223372036854775807")
      return true;
    else if (s == "9223372036854775808")
      return prefix != -1;
    else
      return false;
  }
}

}  // namespace miniplc0
