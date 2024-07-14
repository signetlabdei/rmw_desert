#include "CBorStream.h"

namespace cbor
{

// TX stream

TxStream::TxStream()
{
}

void TxStream::new_packet()
{
  // Initialize packet and writer
  _packet = new uint8_t[MAX_PACKET_LENGTH];
  _writer = new cbor_writer_t;
  
  cbor_writer_init(_writer, _packet, MAX_PACKET_LENGTH);
}

void TxStream::start_transmission(std::string topic_name)
{
  new_packet();
  _overflow = false;
  
  // Topic name
  cbor_encode_text_string(_writer, topic_name.c_str(), topic_name.size());
}

void TxStream::end_transmission()
{
  if (_overflow)
    return;
  
  std::vector<uint8_t> daemon_packet;
    
  for(size_t i=0; i < cbor_writer_len(_writer); i++)
  {
    daemon_packet.push_back(_packet[i]);
  }
  
  TcpDaemon::enqueue_packet(daemon_packet);
  
  delete _packet;
  delete _writer;
}

TxStream & TxStream::operator<<(const uint64_t n)
{
  cbor_error_t result = cbor_encode_unsigned_integer(_writer, n);
  handle_overrun(result);
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
    result = cbor_encode_unsigned_integer(_writer, n);
  else
    result = cbor_encode_negative_integer(_writer, n);
  
  handle_overrun(result);
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
  cbor_error_t result = cbor_encode_float(_writer, f);
  handle_overrun(result);
  return *this;
}

TxStream & TxStream::operator<<(const double d)
{
  cbor_error_t result = cbor_encode_double(_writer, d);
  handle_overrun(result);
  return *this;
}

TxStream & TxStream::operator<<(const std::string s)
{
  cbor_error_t result = cbor_encode_text_string(_writer, s.c_str(), s.size());
  handle_overrun(result);
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

void TxStream::handle_overrun(cbor_error_t result)
{
  if (result == CBOR_OVERRUN)
  {
    _overflow = true;
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
	      char buf[16];
              int len = sprintf(buf, "%d", val.i32);
              buf[len] = '\0';
              printf("int value: %d\n", atoi(buf));
	      } break;
        case CBOR_ITEM_STRING:
	      printf("string value: %.*s\n", items[i].size, val.str_copy);
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
