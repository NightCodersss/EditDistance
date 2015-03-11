#ifndef _CHARTYPE_HPP_
#define _CHARTYPE_HPP_

#include <vector>
#include <cstdint>

using char_type = int;
const char_type EPS = -1;

const int32_t EPS_char = 0x03b5;

using string_type = std::vector<char_type>;

const int MAX_PATHS_SIZE = 100000000;

#endif
