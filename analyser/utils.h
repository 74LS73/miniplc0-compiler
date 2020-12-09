#pragma once

#include <cstddef>  // for std::size_t
#include <cstdint>
#include <string>

namespace miniplc0 {
int64_t StringToUnsignedInt64(std::string s);
double StringToUnsignedDouble(std::string s);
std::string RemoveIntPrefixZero(std::string s);
bool IsSignedInt64Overflow(std::string s, int prefix);

}  // namespace miniplc0
