#ifndef SEC_UTILS_H
#define SEC_UTILS_H

#include <string>
#include <vector>
#include <cstdint>

bool decode_hex(const std::string& hex, std::vector<uint8_t>& out);

#endif
