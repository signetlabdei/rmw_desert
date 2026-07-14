#include "CBorStream.h"

#include <nanocbor/nanocbor.h>

#ifdef SECURE_MODE_ENABLED
#include "security/SecurityLayer.h"
#endif

namespace cbor
{

#ifdef SECURE_MODE_ENABLED
static security::SecurityLayer g_sec_layer{};
#endif

// TX stream

// Forward declarations to abstract the implementation
struct TxStream::WRITER
{
  nanocbor_encoder_t nanocbor_encoder;
};

struct TxStream::ERROR
{
  nanocbor_error_t nanocbor_error;
};

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
  _writer = new WRITER;

  nanocbor_encoder_init(&_writer->nanocbor_encoder, _packet, MAX_PACKET_LENGTH);
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
  {
    delete _packet;
    delete _writer;
    return;
  }

  size_t encoded_len = nanocbor_encoded_len(&_writer->nanocbor_encoder);

  size_t wrapped_size = encoded_len;
  uint8_t* wrapped_ptr = _packet;

#ifdef SECURE_MODE_ENABLED
  auto st = g_sec_layer.wrap(_packet, encoded_len, MAX_PACKET_LENGTH, &wrapped_ptr, &wrapped_size);
  if (st != security::OK)
  {
    delete _packet;
    delete _writer;
    return;
  }
#endif

  std::vector<uint8_t> daemon_packet(wrapped_ptr, wrapped_ptr + wrapped_size);
  TcpDaemon::enqueue_packet(daemon_packet);

  delete _packet;
  delete _writer;
}

TxStream & TxStream::operator<<(const uint64_t n)
{
  nanocbor_error_t result = (nanocbor_error_t) nanocbor_fmt_uint(&_writer->nanocbor_encoder, n);
  handle_overrun(ERROR(result));
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
  nanocbor_error_t result;

  if (n >= 0)
  {
    result = (nanocbor_error_t) nanocbor_fmt_uint(&_writer->nanocbor_encoder, n);
  }
  else
  {
    result = (nanocbor_error_t) nanocbor_fmt_int(&_writer->nanocbor_encoder, n);
  }

  handle_overrun(ERROR(result));

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
  nanocbor_error_t result = (nanocbor_error_t) nanocbor_fmt_float(&_writer->nanocbor_encoder, f);
  handle_overrun(ERROR(result));
  return *this;
}

TxStream & TxStream::operator<<(const double d)
{
  *this << static_cast<float>(d);
  return *this;
}

TxStream & TxStream::operator<<(const std::string s)
{
  nanocbor_error_t result = (nanocbor_error_t) nanocbor_put_tstrn(&_writer->nanocbor_encoder, s.c_str(), s.size());
  handle_overrun(ERROR(result));
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
  *this << static_cast<uint8_t>(b);
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

void TxStream::handle_overrun(ERROR result)
{
  if (result.nanocbor_error == NANOCBOR_ERR_END)
  {
    _overflow = true;
  }
}

std::string TxStream::toUTF8(const std::u16string source)
{
    std::string result;

    std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> convertor;
    result = convertor.to_bytes(source);

    return result;
}


// RX stream

// Forward declarations to abstract the implementation
struct RxStream::ITEM
{
  nanocbor_value_t nanocbor_value;
};

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
  if (_buffered_packet.size() > _buffered_iterator && _buffered_packet[_buffered_iterator].second == NANOCBOR_TYPE_UINT)
    n = *static_cast<uint64_t *>(_buffered_packet[_buffered_iterator].first);

  _buffered_iterator++;

  return *this;
}

RxStream & RxStream::operator>>(uint32_t & n)
{
  uint64_t value;
  *this >> value;
  n = static_cast<uint32_t>(value);
  return *this;
}

RxStream & RxStream::operator>>(uint16_t & n)
{
  uint64_t value;
  *this >> value;
  n = static_cast<uint16_t>(value);
  return *this;
}

RxStream & RxStream::operator>>(uint8_t & n)
{
  uint64_t value;
  *this >> value;
  n = static_cast<uint8_t>(value);
  return *this;
}

RxStream & RxStream::operator>>(int64_t & n)
{
  int type = _buffered_packet[_buffered_iterator].second;
  if (_buffered_packet.size() > _buffered_iterator && (type == NANOCBOR_TYPE_UINT || type == NANOCBOR_TYPE_NINT))
    n = *static_cast<int64_t *>(_buffered_packet[_buffered_iterator].first);

  _buffered_iterator++;

  return *this;
}

RxStream & RxStream::operator>>(int32_t & n)
{
  int64_t value;
  *this >> value;
  n = static_cast<int32_t>(value);
  return *this;
}

RxStream & RxStream::operator>>(int16_t & n)
{
  int64_t value;
  *this >> value;
  n = static_cast<int16_t>(value);
  return *this;
}

RxStream & RxStream::operator>>(int8_t & n)
{
  int64_t value;
  *this >> value;
  n = static_cast<int8_t>(value);
  return *this;
}

RxStream & RxStream::operator>>(char & n)
{
  if (_buffered_packet.size() > _buffered_iterator && _buffered_packet[_buffered_iterator].second == NANOCBOR_TYPE_TSTR)
    n = (*static_cast<std::string *>(_buffered_packet[_buffered_iterator].first))[0];

  _buffered_iterator++;

  return *this;
}

RxStream & RxStream::operator>>(float & f)
{
  if (_buffered_packet.size() > _buffered_iterator && _buffered_packet[_buffered_iterator].second == NANOCBOR_TYPE_FLOAT)
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
  if (_buffered_packet.size() > _buffered_iterator && _buffered_packet[_buffered_iterator].second == NANOCBOR_TYPE_TSTR)
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

  if (_buffered_packet.size() > _buffered_iterator && _buffered_packet[_buffered_iterator].second == NANOCBOR_TYPE_UINT)
  {
    b_ = *static_cast<int8_t *>(_buffered_packet[_buffered_iterator].first);
    b = b_ ? true : false;

    _buffered_iterator++;
  }

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
    size_t unwrapped_size = packet.size();

#ifdef SECURE_MODE_ENABLED
    auto unwrap_st = g_sec_layer.unwrap(buffer, packet.size(), &unwrapped_size);
    if (unwrap_st != security::OK)
    {
      continue;
    }
#endif

    ITEM decoder;
    nanocbor_decoder_init(&decoder.nanocbor_value, buffer, unwrapped_size);

    uint8_t stream_type;
    uint8_t stream_identifier;
    std::string stream_name;

    std::vector<std::pair<void *, int>> interpreted_packet;

    for (size_t i = 0; !nanocbor_at_end(&decoder.nanocbor_value); i++)
    {
      union _cbor_value val;

      if (i == 0)
      {
        if (nanocbor_get_uint8(&decoder.nanocbor_value, &stream_type) < 0)
        {
          break;
        }
      }
      else if (i == 1)
      {
        if (nanocbor_get_uint8(&decoder.nanocbor_value, &stream_identifier) < 0)
        {
          break;
        }

        stream_name = TopicsConfig::get_identifier_topic(stream_identifier);

        if (stream_name.empty())
        {
          break;
        }
      }
      else
      {
        std::pair<void *, int> field = interpret_field(&decoder, 0, val);

        if (field.first)
        {
          interpreted_packet.push_back(field);
        }
      }
    }

    if (stream_name.empty())
    {
      for (auto& field : interpreted_packet)
      {
        if (field.first)
        {
          free(field.first);
          field.first = nullptr;
        }
      }
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

std::pair<void *, int> RxStream::interpret_field(ITEM * cbor_value, size_t i, union _cbor_value & val)
{
  (void) i;
  (void) val;
  int cbor_value_type = nanocbor_get_type(&cbor_value->nanocbor_value);
  switch (cbor_value_type)
  {
    case NANOCBOR_TYPE_NINT:
    {
      int64_t val;
      if (nanocbor_get_int64(&cbor_value->nanocbor_value, &val) < 0)
      {
        break;
      }
      
      int64_t * number = new int64_t{val};
      return std::make_pair(number, NANOCBOR_TYPE_NINT);
    }
    case NANOCBOR_TYPE_UINT:
    {
      uint64_t val;
      if (nanocbor_get_uint64(&cbor_value->nanocbor_value, &val) < 0)
      {
        break;
      }
      
      uint64_t * number = new uint64_t{val};
      return std::make_pair(number, NANOCBOR_TYPE_UINT);
    }
    case NANOCBOR_TYPE_FLOAT:
    {
      float val;
      if (nanocbor_get_float(&cbor_value->nanocbor_value, &val) < 0)
      {
        break;
      }
      
      float * f = new float{val};
      return std::make_pair(static_cast<void *>(f), NANOCBOR_TYPE_FLOAT);
    }
    case NANOCBOR_TYPE_TSTR:
    {
      const char * val;
      size_t val_size;
      if (nanocbor_get_tstr(&cbor_value->nanocbor_value, (const uint8_t**) &val, &val_size) < 0)
      {
        break;
      }
      
      std::string * s = new std::string(val, val_size);
      return std::make_pair(s, NANOCBOR_TYPE_TSTR);
    }
    case NANOCBOR_TYPE_BSTR:
    {
      const char* val;
      size_t val_size;
      if (nanocbor_get_bstr(&cbor_value->nanocbor_value, (const uint8_t**) &val, &val_size) < 0)
      {
        break;
      }
      
      std::string * s = new std::string(val, val_size);
      return std::make_pair(s, NANOCBOR_TYPE_TSTR);
    }
    default:
      nanocbor_skip(&cbor_value->nanocbor_value);
      return std::make_pair(nullptr, 0);
  }
  
  nanocbor_skip(&cbor_value->nanocbor_value);
  return std::make_pair(nullptr, 0);
}

std::u16string RxStream::toUTF16(const std::string source)
{
    std::u16string result;

    std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> convertor;
    result = convertor.from_bytes(source);

    return result;
}

}
