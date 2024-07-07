#include "CBorStream.h"

namespace cbor
{

TxStream::TxStream(size_t hint)
{
}

TxStream & TxStream::operator<<(const uint64_t n)
{
  return *this;
}

TxStream & TxStream::operator<<(const uint32_t n)
{
  return *this;
}

TxStream & TxStream::operator<<(const uint16_t n)
{
  return *this;
}

TxStream & TxStream::operator<<(const uint8_t n)
{
  return *this;
}

TxStream & TxStream::operator<<(const int64_t n)
{
  return *this;
}

TxStream & TxStream::operator<<(const int32_t n)
{
  return *this;
}

TxStream & TxStream::operator<<(const int16_t n)
{
  return *this;
}

TxStream & TxStream::operator<<(const int8_t n)
{
  return *this;
}

TxStream & TxStream::operator<<(const float f)
{
  return *this;
}

TxStream & TxStream::operator<<(const double d)
{
  return *this;
}

TxStream & TxStream::operator<<(const std::string s)
{
  printf("INCOMING MESSAGE: %s\n", s.c_str());
  return *this;
}

TxStream & TxStream::operator<<(const std::u16string s)
{
  return *this;
}

}
