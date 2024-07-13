#include "CBorStream.h"

namespace cbor
{

// TX stream

TxStream::TxStream()
{
}

void TxStream::add_packet()
{
  int packet_sequence_number = _packets.size();

  // Initialize packet and writer
  uint8_t * packet = new uint8_t[MAX_PACKET_LENGTH];
  _packets.push_back(packet);
  cbor_writer_t * writer = new cbor_writer_t;
  _writers.push_back(writer);
  
  cbor_writer_init(_writers.back(), _packets[packet_sequence_number], MAX_PACKET_LENGTH);
  
  // Sequence identifier
  cbor_encode_unsigned_integer(_writers.back(), _sequence_id);
  // Sequence number identifier
  cbor_encode_unsigned_integer(_writers.back(), packet_sequence_number);
}

void TxStream::start_transmission(std::string topic_name)
{
  _sequence_id = std::rand();
  add_packet();
  
  // Topic name
  cbor_encode_text_string(_writers.back(), topic_name.c_str(), topic_name.size());
}

void TxStream::end_transmission()
{
  for(size_t c=0; c < _packets.size(); c++)
  {
    std::vector<uint8_t> daemon_packet;
    
    for(size_t i=0; i < cbor_writer_len(_writers[c]); i++)
    {
      daemon_packet.push_back(_packets[c][i]);
    }
    
    TcpDaemon::enqueue_packet(daemon_packet);
  }
  
  _packets.clear();
  _writers.clear();
}

TxStream & TxStream::operator<<(const uint64_t n)
{
  cbor_error_t result = cbor_encode_unsigned_integer(_writers.back(), n);
  handle_overrun<uint64_t>(result, n);
  return *this;
}

TxStream & TxStream::operator<<(const uint32_t n)
{
  *this << static_cast<uint64_t>(n);
  return *this;
}

TxStream & TxStream::operator<<(const uint16_t n)
{
  *this << static_cast<uint64_t>(n);
  return *this;
}

TxStream & TxStream::operator<<(const uint8_t n)
{
  *this << static_cast<uint64_t>(n);
  return *this;
}

TxStream & TxStream::operator<<(const int64_t n)
{
  cbor_error_t result;
  
  if (n >= 0)
    result = cbor_encode_unsigned_integer(_writers.back(), n);
  else
    result = cbor_encode_negative_integer(_writers.back(), n);
  
  handle_overrun<int64_t>(result, n);
  return *this;
}

TxStream & TxStream::operator<<(const int32_t n)
{
  *this << static_cast<int64_t>(n);
  return *this;
}

TxStream & TxStream::operator<<(const int16_t n)
{
  *this << static_cast<int64_t>(n);
  return *this;
}

TxStream & TxStream::operator<<(const int8_t n)
{
  *this << static_cast<int64_t>(n);
  return *this;
}

TxStream & TxStream::operator<<(const float f)
{
  cbor_error_t result = cbor_encode_float(_writers.back(), f);
  handle_overrun<float>(result, f);
  return *this;
}

TxStream & TxStream::operator<<(const double d)
{
  cbor_error_t result = cbor_encode_double(_writers.back(), d);
  handle_overrun<double>(result, d);
  return *this;
}

TxStream & TxStream::operator<<(const std::string s)
{
  cbor_error_t result = cbor_encode_text_string(_writers.back(), s.c_str(), s.size());
  handle_overrun<std::string>(result, s);
  return *this;
}

TxStream & TxStream::operator<<(const std::u16string s)
{
  // Cbor does not support UTF-16 so a conversion to UTF-8 is performed
  *this << toUTF8(s);
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
    add_packet();
    *this << parameter;
  }
}

std::string TxStream::toUTF8(const std::u16string source)
{
    std::string result;

    std::wstring_convert<std::codecvt_utf8_utf16<std::u16string >, std::u16string> convertor;
    result = convertor.to_bytes(source);

    return result;
}


// RX stream

RxStream::RxStream(std::string topic_name)
      : _topic_name(topic_name)
{
}
std::map<std::string, std::vector<uint8_t>> RxStream::_defragmented_packets;

RxStream & RxStream::operator>>(const void ** data)
{
  return *this;
}

void RxStream::defragment_packets()
{
  std::vector<uint8_t> packet;
  for (packet = TcpDaemon::read_packet(); packet.size() != 0; packet = TcpDaemon::read_packet())
  {
    printf("There is a packet\n");
    for(int i=0; i < packet.size(); i++)
        printf("%02x ", packet[i]);
    printf("\n");
    
    // Initialize buffer and reader
    uint8_t * buffer = &packet[0];
    cbor_reader_t reader;
    cbor_item_t items[16];
    size_t n;

    cbor_reader_init(&reader, items, sizeof(items) / sizeof(items[0]));
    cbor_parse(&reader, buffer, packet.size(), &n);
    
    for (size_t i = 0; i < n; i++)
    {
      union _cbor_value val;

      memset(&val, 0, sizeof(val));
      cbor_decode(&reader, &items[i], &val, sizeof(val));

      switch (items[i].type)
      {
        case CBOR_ITEM_INTEGER: {
	      printf("int\n");
	      } break;
        case CBOR_ITEM_STRING:
	      printf("string\n");
	      break;
        case CBOR_ITEM_SIMPLE_VALUE:
	      printf("bool\n");
        default:
	      break;
      }
    }
  }
}

void RxStream::interpret_packets()
{
  
}

}
