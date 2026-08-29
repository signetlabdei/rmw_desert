#ifndef SEC_UTILS_H
#define SEC_UTILS_H

#include <string>
#include <vector>
#include <cstdint>

#define MIN(a, b) ((a) > (b) ? (b) : (a))

bool decode_hex(const std::string& hex, std::vector<uint8_t>& out);

#endif
