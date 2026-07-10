#include "sec_utils.h"

static int hex_value(char c)
{
  if (c >= '0' && c <= '9') return c - '0';
  if (c >= 'a' && c <= 'f') return 10 + (c - 'a');
  if (c >= 'A' && c <= 'F') return 10 + (c - 'A');
  return -1;
}

bool decode_hex(const std::string& hex, std::vector<uint8_t>& out)
{
  if (hex.size() % 2 != 0)
  {
    return false;
  }

  out.clear();
  out.reserve(hex.size() / 2);

  for (size_t i = 0; i < hex.size(); i += 2)
  {
    const int hi = hex_value(hex[i]);
    const int lo = hex_value(hex[i + 1]);
    if (hi < 0 || lo < 0)
    {
      out.clear();
      return false;
    }

    out.push_back(static_cast<uint8_t>((hi << 4) | lo));
  }

  return true;
}
