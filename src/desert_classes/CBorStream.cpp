#include "CBorStream.h"

namespace cbor
{

TxStream::TxStream()
{
}

void TxStream::add_packet()
{
  int packet_sequence_number = _packets.size();

  // Initialize packet and writer
  uint8_t * packet = new uint8_t[125];
  _packets.push_back(packet);
  cbor_writer_t * writer = new cbor_writer_t;
  _writers.push_back(writer);
  
  cbor_writer_init(_writers.back(), _packets[packet_sequence_number], 125);
  
  // Sequence identifier
  cbor_encode_unsigned_integer(_writers.back(), _sequence_id);
  // Sequence number identifier
  cbor_encode_unsigned_integer(_writers.back(), packet_sequence_number);
}

void TxStream::start_transmission(std::string topic_name)
{
  printf("Start transmission, topic: %s\n", topic_name.c_str());
  
  _sequence_id = std::rand();
  add_packet();
  
  // Topic name
  cbor_encode_text_string(_writers.back(), topic_name.c_str(), topic_name.size());
}

void TxStream::end_transmission()
{
  printf("End transmission, encoded data: \n");
  
  for(int c=0; c < _packets.size(); c++)
  {
    printf("%i: ", c);
    for(size_t i=0; i < cbor_writer_len(_writers[c]); i++)
        printf("%02x ", _packets[c][i]);
    printf("\n");
  }
  printf("\n");
  
  _packets.clear();
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
  cbor_error_t result = cbor_encode_text_string(_writers.back(), s.c_str(), s.size());
  handle_overrun<std::string>(result, s);
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

template<typename T>
void TxStream::handle_overrun(cbor_error_t result, const T parameter)
{
  if (result == CBOR_OVERRUN)
  {
    printf("Overrun occurred\n");
    add_packet();
    *this << parameter;
  }
}

}
