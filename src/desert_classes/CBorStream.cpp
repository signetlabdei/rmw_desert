#include "CBorStream.h"

namespace cbor
{

// TX stream

TxStream::TxStream(uint8_t stream_type, std::string stream_name, uint8_t stream_identifier)
      : _stream_type(stream_type)
      , _stream_name(stream_name)
      , _stream_identifier(stream_identifier)
{
}

void TxStream::new_packet()
{
  // Initialize packet and writer
  _packet = new uint8_t[MAX_PACKET_LENGTH];
  _writer = new cbor_writer_t;
  
  cbor_writer_init(_writer, _packet, MAX_PACKET_LENGTH);
}

void TxStream::start_transmission(uint64_t sequence_id)
{
  new_packet();
  _overflow = false;
  
  // Stream type, service name/identifier and sequence id
  *this << _stream_type;
  *this << _stream_identifier;
  *this << sequence_id;
}

void TxStream::start_transmission()
{
  new_packet();
  _overflow = false;
  
  // Stream type and topic name/identifier
  *this << _stream_type;
  *this << _stream_identifier;
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

TxStream & TxStream::operator<<(const char n)
{
  std::string single_char_string(1, n);
  *this << single_char_string;
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
  *this << static_cast<float>(d);
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

TxStream & TxStream::operator<<(const bool b)
{
  cbor_error_t result = cbor_encode_bool(_writer, b);
  handle_overrun(result);
  return *this;
}

TxStream & TxStream::operator<<(const std::vector<bool> v)
{
  *this << static_cast<uint32_t>(v.size());
  for (size_t i = 0; i < v.size(); ++i)
  {
    *this << v[i];
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
  result.reserve(source.size());

  const auto appendByte = [&result](std::uint32_t byte) {
    result.push_back(static_cast<char>(byte));
  };

  for (std::size_t i = 0; i < source.size(); ++i)
  {
    std::uint32_t cp = source[i];

    if (cp >= 0xD800 && cp <= 0xDBFF)
    {
      // UTF-16: incomplete surrogate pair
      if (i + 1 == source.size())
      {
        return "";
      }

      const std::uint32_t low = source[i + 1];

      // UTF-16: invalid surrogate pair
      if (low < 0xDC00 || low > 0xDFFF)
      {
        return "";
      }

      cp = 0x10000
      + ((cp - 0xD800) << 10)
      + (low - 0xDC00);

      ++i;
    }
    // UTF-16: isolated low surrogate
    else if (cp >= 0xDC00 && cp <= 0xDFFF)
    {
      return "";
    }

    if (cp <= 0x7F)
    {
      appendByte(cp);
    }
    else if (cp <= 0x7FF)
    {
      appendByte(0xC0 | (cp >> 6));
      appendByte(0x80 | (cp & 0x3F));
    }
    else if (cp <= 0xFFFF)
    {
      appendByte(0xE0 | (cp >> 12));
      appendByte(0x80 | ((cp >> 6) & 0x3F));
      appendByte(0x80 | (cp & 0x3F));
    }
    else
    {
      appendByte(0xF0 | (cp >> 18));
      appendByte(0x80 | ((cp >> 12) & 0x3F));
      appendByte(0x80 | ((cp >> 6) & 0x3F));
      appendByte(0x80 | (cp & 0x3F));
    }
  }

  return result;
}


// RX stream

RxStream::RxStream(uint8_t stream_type, std::string stream_name, uint8_t stream_identifier)
      : _stream_type(stream_type)
      , _stream_name(stream_name)
      , _stream_identifier(stream_identifier)
{
  _listening_streams.push_back(this);
}

RxStream::~RxStream()
{
  std::vector<RxStream *>::iterator position = std::find(_listening_streams.begin(), _listening_streams.end(), this);
  if (position != _listening_streams.end())
    _listening_streams.erase(position);
}

const std::map<int, int> RxStream::_stream_type_match_map = {
  { PUBLISHER_TYPE, SUBSCRIBER_TYPE },
  { CLIENT_TYPE,    SERVICE_TYPE    },
  { SERVICE_TYPE,   CLIENT_TYPE     }
};

std::vector<RxStream *> RxStream::_listening_streams;

std::mutex RxStream::_rx_mutex;

bool RxStream::data_available(int64_t sequence_id)
{
  // If a packet is already buffered, so do not overwrite it and wait for a clear
  if (_buffered_packet.size() > 0)
    return true;
  
  // No buffered packets, go on checking for other ones
  if (_received_packets.size() == 0)
    return false;
  
  // The received packets queue is not empty, so examine it
  if (sequence_id)
  {
    for (size_t i = 0; i < _received_packets.size(); i++)
    {
      // Check for the first element in the packet, which is the sequence id
      if (*static_cast<int64_t *>(_received_packets.front().at(0).first) == sequence_id)
      {
        _buffered_packet = _received_packets.front();
        _received_packets.pop();
        _buffered_iterator = 1;
        return true;
      }
      else
      {
        _received_packets.push(_received_packets.front());
        _received_packets.pop();
      }
    }
    return false;
  }
  else
  {
    _buffered_packet = _received_packets.front();
    _received_packets.pop();
    _buffered_iterator = 0;
    return true;
  }
}

void RxStream::clear_buffer()
{
  _buffered_packet.clear();
}

RxStream & RxStream::operator>>(uint64_t & n)
{
  return deserialize_integer<uint64_t>(n);
}

RxStream & RxStream::operator>>(uint32_t & n)
{
  return deserialize_integer<uint32_t>(n);
}

RxStream & RxStream::operator>>(uint16_t & n)
{
  return deserialize_integer<uint16_t>(n);
}

RxStream & RxStream::operator>>(uint8_t & n)
{
  return deserialize_integer<uint8_t>(n);
}

RxStream & RxStream::operator>>(int64_t & n)
{
  return deserialize_integer<int64_t>(n);
}

RxStream & RxStream::operator>>(int32_t & n)
{
  return deserialize_integer<int32_t>(n);
}

RxStream & RxStream::operator>>(int16_t & n)
{
  return deserialize_integer<int16_t>(n);
}

RxStream & RxStream::operator>>(int8_t & n)
{
  return deserialize_integer<int8_t>(n);
}

template<typename T>
RxStream & RxStream::deserialize_integer(T & n)
{
  if (_buffered_packet.size() > _buffered_iterator && _buffered_packet[_buffered_iterator].second == CBOR_ITEM_INTEGER)
    n = *static_cast<T *>(_buffered_packet[_buffered_iterator].first);

  _buffered_iterator++;
  
  return *this;
}

RxStream & RxStream::operator>>(char & n)
{
  if (_buffered_packet.size() > _buffered_iterator && _buffered_packet[_buffered_iterator].second == CBOR_ITEM_STRING)
    n = (*static_cast<std::string *>(_buffered_packet[_buffered_iterator].first))[0];

  _buffered_iterator++;
  
  return *this;
}

RxStream & RxStream::operator>>(float & f)
{
  if (_buffered_packet.size() > _buffered_iterator && _buffered_packet[_buffered_iterator].second == CBOR_ITEM_FLOAT)
    f = *static_cast<float *>(_buffered_packet[_buffered_iterator].first);

  _buffered_iterator++;
  
  return *this;
}

RxStream & RxStream::operator>>(double & d)
{
  float value;
  *this >> value;
  d = static_cast<double>(value);
  return *this;
}

RxStream & RxStream::operator>>(std::string & s)
{
  if (_buffered_packet.size() > _buffered_iterator && _buffered_packet[_buffered_iterator].second == CBOR_ITEM_STRING)
    s = *static_cast<std::string *>(_buffered_packet[_buffered_iterator].first);

  _buffered_iterator++;
  
  return *this;
}

RxStream & RxStream::operator>>(std::u16string & s)
{
  std::string str;
  *this >> str;
  
  s = toUTF16(str);
  return *this;
}

RxStream & RxStream::operator>>(bool & b)
{
  int8_t b_;
  
  if (_buffered_packet.size() > _buffered_iterator && _buffered_packet[_buffered_iterator].second == CBOR_ITEM_SIMPLE_VALUE)
    b_ = *static_cast<int8_t *>(_buffered_packet[_buffered_iterator].first);

  b = b_ ? true : false;
  
  _buffered_iterator++;
  
  return *this;
}

RxStream & RxStream::operator>>(std::vector<bool> & v)
{
  uint32_t size;
  *this >> size;
  for (size_t i = 0; i < size; ++i)
  {
    int b;
    *this >> b;
    v[i] = b ? true : false;
  }
  return *this;
}

uint8_t RxStream::get_type() const
{
  return _stream_type;
}

std::string RxStream::get_name() const
{
  return _stream_name;
}

uint8_t RxStream::get_identifier() const
{
  return _stream_identifier;
}

void RxStream::push_packet(std::vector<std::pair<void *, int>> packet)
{
  _received_packets.push(packet);
}

void RxStream::interpret_packets()
{
  std::lock_guard<std::mutex> lock(_rx_mutex);
  
  std::vector<uint8_t> packet;
  for (packet = TcpDaemon::read_packet(); packet.size() != 0; packet = TcpDaemon::read_packet())
  {
    // Initialize buffer and reader
    uint8_t * buffer = &packet[0];
    cbor_reader_t reader;
    cbor_item_t items[64];
    size_t n;

    cbor_reader_init(&reader, items, sizeof(items) / sizeof(items[0]));
    cbor_parse(&reader, buffer, packet.size(), &n);
    
    uint8_t stream_type;
    uint8_t stream_identifier;
    std::string stream_name;
    
    std::vector<std::pair<void *, int>> interpreted_packet;
    
    for (size_t i = 0; i < n; i++)
    {
      union _cbor_value val;

      memset(&val, 0, sizeof(val));
      cbor_decode(&reader, &items[i], &val, sizeof(val));
      
      if (i == 0)
      {
        stream_type = val.i8;
      }
      else if (i == 1)
      {
        stream_identifier = val.i8;
        stream_name = TopicsConfig::get_identifier_topic(stream_identifier);
        
        if (stream_name.empty())
        {
          break;
        }
      }
      else
      {
        std::pair<void *, int> field = interpret_field(items, i, val);
        
        if (field.first)
        {
          interpreted_packet.push_back(field);
        }
      }
    }
    
    if (stream_name.empty())
    {
      continue;
    }
    
    for (RxStream * stream : _listening_streams)
    {
      if (stream->get_type() == _stream_type_match_map.at(stream_type) && stream->get_identifier() == stream_identifier)
      {
        stream->push_packet(interpreted_packet);
      }
    }
  }
}

std::pair<void *, int> RxStream::interpret_field(cbor_item_t * items, size_t i, union _cbor_value & val)
{
  switch (items[i].type)
  {
    case CBOR_ITEM_INTEGER:
    {
      int64_t * number = new int64_t{val.i64};
      
      return std::make_pair(static_cast<void *>(number), CBOR_ITEM_INTEGER);
    }
    case CBOR_ITEM_FLOAT:
    {
      float * number;
      
      if (abs(val.i32) < 65536)
      {
        int16_t * bin = new int16_t{val.i16};
        
        half_float::half * f16 = reinterpret_cast<half_float::half *>(bin);
        number = new float{*f16};
      }
      else
      {
        number = new float{val.f32};
      }
      
      return std::make_pair(static_cast<void *>(number), CBOR_ITEM_FLOAT);
    }
    case CBOR_ITEM_STRING: 
    {
      val.str_copy[items[i].size] = '\0';
      std::string * str = new std::string{reinterpret_cast<const char *>(val.str_copy)};
      
      return std::make_pair(static_cast<void *>(str), CBOR_ITEM_STRING);
    }
    case CBOR_ITEM_SIMPLE_VALUE:
    {
      int8_t * value = new int8_t{val.i8};
      
      return std::make_pair(static_cast<void *>(value), CBOR_ITEM_SIMPLE_VALUE);
    }
    default:
      return std::make_pair(nullptr, 0);
  }
}

std::u16string RxStream::toUTF16(const std::string source)
{
  std::u16string result;
  result.reserve(source.size());

  std::size_t i = 0;

  while (i < source.size())
  {
    const auto first = static_cast<unsigned char>(source[i++]);

    std::uint32_t cp;
    std::uint32_t minimum;
    std::size_t continuationCount;

    if (first <= 0x7F)
    {
      cp = first;
      minimum = 0;
      continuationCount = 0;
    }
    else if (first >= 0xC2 && first <= 0xDF)
    {
      cp = first & 0x1F;
      minimum = 0x80;
      continuationCount = 1;
    }
    else if (first >= 0xE0 && first <= 0xEF)
    {
      cp = first & 0x0F;
      minimum = 0x800;
      continuationCount = 2;
    }
    else if (first >= 0xF0 && first <= 0xF4)
    {
      cp = first & 0x07;
      minimum = 0x10000;
      continuationCount = 3;
    }
    // UTF-8: invalid leading byte
    else
    {
      return u"";
    }

    // UTF-8: incomplete sequence
    if (source.size() - i < continuationCount)
    {
      return u"";
    }

    for (std::size_t j = 0; j < continuationCount; ++j)
    {
      const auto byte = static_cast<unsigned char>(source[i++]);

      // UTF-8: invalid continuation byte
      if ((byte & 0xC0) != 0x80)
      {
        return u"";
      }

      cp = (cp << 6) | (byte & 0x3F);
    }

    // UTF-8: invalid code point
    if (cp < minimum || cp > 0x10FFFF || (cp >= 0xD800 && cp <= 0xDFFF))
    {
      return u"";
    }

    if (cp <= 0xFFFF)
    {
      result.push_back(static_cast<char16_t>(cp));
    }
    else
    {
      cp -= 0x10000;

      result.push_back(static_cast<char16_t>(0xD800 + (cp >> 10)));
      result.push_back(static_cast<char16_t>(0xDC00 + (cp & 0x3FF)));
    }
  }

  return result;
}

}
