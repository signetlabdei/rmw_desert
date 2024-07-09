#include "CBorStream.h"

namespace cbor
{

TxStream::TxStream()
{
}

void TxStream::start_transmission(std::string topic_name)
{
  printf("Start transmission, topic: %s\n", topic_name.c_str());
  
  // Initialize cbor
  _writer = new cbor_writer_t;
  cbor_writer_init(_writer, _packet, sizeof(_packet));
  // Communication type identifier
  cbor_encode_unsigned_integer(_writer, 0);
  // Topic name
  cbor_encode_text_string(_writer, topic_name.c_str(), topic_name.size());
}

void TxStream::end_transmission()
{
  printf("End transmission, encoded data: ");
  
  for(size_t i=0; i < cbor_writer_len(_writer); i++)
      printf("%02x ", _packet[i]);
  printf("\n");
  delete _writer;
}

TxStream & TxStream::operator<<(const uint64_t n)
{
  return *this;
}

TxStream & TxStream::operator<<(const uint32_t n)
{
  printf("INCOMING UINT32: %u\n", n);
  return *this;
}

TxStream & TxStream::operator<<(const uint16_t n)
{
  return *this;
}

TxStream & TxStream::operator<<(const uint8_t n)
{
  printf("INCOMING UINT8: %u\n", n);
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
  printf("INCOMING STRING: %s\n", s.c_str());
  return *this;
}

TxStream & TxStream::operator<<(const std::u16string s)
{
  return *this;
}

template<typename T>
TxStream & TxStream::operator<<(const std::vector<T> v)
{
  return serialize_sequence(v.data(), v.size());
}

TxStream & TxStream::operator<<(const std::vector<bool> v)
{
  for (size_t i = 0; i < v.size(); ++i) {
    *this << v[i];
  }
  return *this;
}

template<typename T>
TxStream & TxStream::serialize_sequence(const T * items, size_t size)
{
  for (size_t i = 0; i < size; ++i) {
    *this << items[i];
  }
  return *this;
}

}
