#include "DcclStream.h"

namespace dccl
{

// Dynamic stream

const google::protobuf::Descriptor * DynamicMessage::_dynamic_descriptor;

DynamicStream::DynamicStream(std::string protobuf)
      : _proto(protobuf)
      , _pool(DescriptorPool::generated_pool())
{
  if (_proto == "") 
  {
    _internal_state = RMW_RET_ERROR;
    return;
  }
  
  DCCL_DEBUG("Proto: \n%s\n\n", _proto.c_str());
  
  _internal_state = get_message_descriptor();
  _internal_state = get_mutable_message();
  _internal_state = get_reflection();
}

rmw_ret_t DynamicStream::get_message_descriptor()
{
  std::string message_type("DynamicMessage");
 
  ArrayInputStream raw_input(_proto.c_str(), strlen(_proto.c_str()));
  Tokenizer input(&raw_input, NULL);
 
  // Proto definition to a representation as used by the protobuf lib:
  /* FileDescriptorProto documentation:
   * A valid .proto file can be translated directly to a FileDescriptorProto
   * without any other information (e.g. without reading its imports).
   * */
  _file_desc_proto.add_dependency("dccl/option_extensions.proto");
  Parser parser;
  if (!parser.Parse(&input, &_file_desc_proto))
    return RMW_RET_ERROR;
 
  // Set the name in _file_desc_proto as Parser::Parse does not do this:
  if (!_file_desc_proto.has_name())
    _file_desc_proto.set_name(message_type);
 
  // Construct our own FileDescriptor for the proto file:
  /* FileDescriptor documentation:
   * Describes a whole .proto file.  To get the FileDescriptor for a compiled-in
   * file, get the descriptor for something defined in that file and call
   * descriptor->file().  Use DescriptorPool to construct your own descriptors.
   * */
  _file_desc = _pool.BuildFile(_file_desc_proto);
  if (_file_desc == NULL)
    return RMW_RET_ERROR;
 
  // As a .proto definition can contain more than one message Type,
  // select the message type that we are interested in
  _message_descs.push(_file_desc->FindMessageTypeByName(message_type));
  
  if (_message_descs.top() == NULL)
  {
    std::cout << "Cannot get message descriptor";
    return RMW_RET_ERROR;
  }
  
  return RMW_RET_OK;
}

rmw_ret_t DynamicStream::get_mutable_message()
{
  // Create an empty Message object that will hold the result
  _prototype_msg = _factory.GetPrototype(_message_descs.top());
  
  if (_prototype_msg == NULL)
  {
    std::cout << "Cannot create prototype message from message descriptor";
    return RMW_RET_ERROR;
  }
  
  _mutable_msgs.push(_prototype_msg->New());
  
  if (_mutable_msgs.top() == NULL)
  {
    std::cout << "Failed to create mutable message";
    return RMW_RET_ERROR;
  }
  
  return RMW_RET_OK;
}

rmw_ret_t DynamicStream::get_reflection()
{
  // Use the _reflection interface to examine the contents
  _reflections.push(_mutable_msgs.top()->GetReflection());
  
  if (_prototype_msg == NULL)
  {
    std::cout << "Cannot create prototype message from message descriptor";
    return RMW_RET_ERROR;
  }
  
  return RMW_RET_OK;
}

const std::map<std::type_index, int> DynamicStream::_types_map = {
  { typeid(uint64_t),    FieldDescriptor::CppType::CPPTYPE_UINT64 },
  { typeid(uint32_t),    FieldDescriptor::CppType::CPPTYPE_UINT32 },
  { typeid(int64_t),     FieldDescriptor::CppType::CPPTYPE_INT64  },
  { typeid(int32_t),     FieldDescriptor::CppType::CPPTYPE_INT32  },
  { typeid(float),       FieldDescriptor::CppType::CPPTYPE_FLOAT  },
  { typeid(double),      FieldDescriptor::CppType::CPPTYPE_DOUBLE },
  { typeid(std::string), FieldDescriptor::CppType::CPPTYPE_STRING },
  { typeid(bool),        FieldDescriptor::CppType::CPPTYPE_BOOL   }
};

const std::map<int, int> DynamicStream::_stream_type_match_map = {
  { PUBLISHER_TYPE, SUBSCRIBER_TYPE },
  { CLIENT_TYPE,    SERVICE_TYPE    },
  { SERVICE_TYPE,   CLIENT_TYPE     }
};

// TX stream

TxStream::TxStream(uint8_t stream_type, std::string stream_name, uint8_t stream_identifier, std::string proto)
      : DynamicStream(proto)
      , _stream_type(stream_type)
      , _stream_name(stream_name)
      , _stream_identifier(stream_identifier)
{
}

void TxStream::start_transmission(uint64_t sequence_id)
{
  _counters.push(1);
  
  // Stream type, service name/identifier and sequence id
  *this << _stream_type;
  *this << sequence_id;
}

void TxStream::start_transmission()
{
  _counters.push(1);
  
  // Stream type
  *this << _stream_type;
}

void TxStream::end_transmission()
{
  std::vector<uint8_t> daemon_packet;
  
  DynamicMessage::_dynamic_descriptor = _message_descs.top();
  _codec.load<DynamicMessage>();
  _codec.encode(&_encoded_bytes, *_mutable_msgs.top());
  
  for(size_t i=0; i < _encoded_bytes.length(); i++)
  {
    daemon_packet.push_back(static_cast<uint8_t>(_encoded_bytes[i]));
    //std::cout << std::bitset<8>(_encoded_bytes[i]) << " ";
  }
  
  DCCL_DEBUG("TX incoming:\n%s\n", _mutable_msgs.top()->DebugString().c_str());
  
  _encoded_bytes.clear();
  TcpDaemon::enqueue_packet(daemon_packet);
}

TxStream & TxStream::operator<<(const uint64_t n)
{
  return encode_field<uint64_t>(
    [](const Reflection * reflection, Message * message, const FieldDescriptor * field, const uint64_t n){ reflection->SetUInt64(message, field, n); }, n);
}

TxStream & TxStream::operator<<(const uint32_t n)
{
  return encode_field<uint32_t>(
    [](const Reflection * reflection, Message * message, const FieldDescriptor * field, const uint32_t n){ reflection->SetUInt32(message, field, n); }, n);
}

TxStream & TxStream::operator<<(const uint16_t n)
{
  *this << static_cast<uint32_t>(n);
  return *this;
}

TxStream & TxStream::operator<<(const uint8_t n)
{
  *this << static_cast<uint32_t>(n);
  return *this;
}

TxStream & TxStream::operator<<(const int64_t n)
{
  return encode_field<int64_t>(
    [](const Reflection * reflection, Message * message, const FieldDescriptor * field, const int64_t n){ reflection->SetInt64(message, field, n); }, n);
}

TxStream & TxStream::operator<<(const int32_t n)
{
  return encode_field<int32_t>(
    [](const Reflection * reflection, Message * message, const FieldDescriptor * field, const int32_t n){ reflection->SetInt32(message, field, n); }, n);
}

TxStream & TxStream::operator<<(const int16_t n)
{
  *this << static_cast<int32_t>(n);
  return *this;
}

TxStream & TxStream::operator<<(const int8_t n)
{
  *this << static_cast<int32_t>(n);
  return *this;
}

TxStream & TxStream::operator<<(const char n)
{
  *this << static_cast<uint32_t>(n);
  return *this;
}

TxStream & TxStream::operator<<(const float f)
{
  return encode_field<float>(
    [](const Reflection * reflection, Message * message, const FieldDescriptor * field, const float f){ reflection->SetFloat(message, field, f); }, f);
}

TxStream & TxStream::operator<<(const double d)
{
  return encode_field<double>(
    [](const Reflection * reflection, Message * message, const FieldDescriptor * field, const double d){ reflection->SetDouble(message, field, d); }, d);
}

TxStream & TxStream::operator<<(const std::string s)
{
  return encode_field<std::string>(
    [](const Reflection * reflection, Message * message, const FieldDescriptor * field, const std::string s){ reflection->SetString(message, field, s); }, s);
}

TxStream & TxStream::operator<<(const std::u16string s)
{
  // DCCL does not support UTF-16 so a conversion to UTF-8 is performed
  *this << toUTF8(s);
  return *this;
}

TxStream & TxStream::operator<<(const bool b)
{
  return encode_field<bool>(
    [](const Reflection * reflection, Message * message, const FieldDescriptor * field, const bool b){ reflection->SetBool(message, field, b); }, b);
}

TxStream & TxStream::operator<<(const std::vector<uint64_t> v)
{
  return encode_vector<uint64_t>(
    [](const Reflection * reflection, Message * message, const FieldDescriptor * field, const uint64_t val){ reflection->AddUInt64(message, field, val); }, v);
}

TxStream & TxStream::operator<<(const std::vector<uint32_t> v)
{
  return encode_vector<uint32_t>(
    [](const Reflection * reflection, Message * message, const FieldDescriptor * field, const uint32_t val){ reflection->AddUInt32(message, field, val); }, v);
}

TxStream & TxStream::operator<<(const std::vector<uint16_t> v)
{
  return ucast_encode_vector<uint16_t>(
    [](const Reflection * reflection, Message * message, const FieldDescriptor * field, const uint32_t val){ reflection->AddUInt32(message, field, val); }, v);
}

TxStream & TxStream::operator<<(const std::vector<uint8_t> v)
{
  return ucast_encode_vector<uint8_t>(
    [](const Reflection * reflection, Message * message, const FieldDescriptor * field, const uint32_t val){ reflection->AddUInt32(message, field, val); }, v);
}

TxStream & TxStream::operator<<(const std::vector<int64_t> v)
{
  return encode_vector<int64_t>(
    [](const Reflection * reflection, Message * message, const FieldDescriptor * field, const int64_t val){ reflection->AddInt64(message, field, val); }, v);
}

TxStream & TxStream::operator<<(const std::vector<int32_t> v)
{
  return encode_vector<int32_t>(
    [](const Reflection * reflection, Message * message, const FieldDescriptor * field, const int32_t val){ reflection->AddInt32(message, field, val); }, v);
}

TxStream & TxStream::operator<<(const std::vector<int16_t> v)
{
  return cast_encode_vector<int16_t>(
    [](const Reflection * reflection, Message * message, const FieldDescriptor * field, const int32_t val){ reflection->AddInt32(message, field, val); }, v);
}

TxStream & TxStream::operator<<(const std::vector<int8_t> v)
{
  return cast_encode_vector<int8_t>(
    [](const Reflection * reflection, Message * message, const FieldDescriptor * field, const int32_t val){ reflection->AddInt32(message, field, val); }, v);
}

TxStream & TxStream::operator<<(const std::vector<char> v)
{
  return ucast_encode_vector<char>(
    [](const Reflection * reflection, Message * message, const FieldDescriptor * field, const uint32_t val){ reflection->AddUInt32(message, field, val); }, v);
}

TxStream & TxStream::operator<<(const std::vector<float> v)
{
  return encode_vector<float>(
    [](const Reflection * reflection, Message * message, const FieldDescriptor * field, const float val){ reflection->AddFloat(message, field, val); }, v);
}

TxStream & TxStream::operator<<(const std::vector<double> v)
{
  return encode_vector<double>(
    [](const Reflection * reflection, Message * message, const FieldDescriptor * field, const double val){ reflection->AddDouble(message, field, val); }, v);
}

TxStream & TxStream::operator<<(const std::vector<std::string> v)
{
  return encode_vector<std::string>(
    [](const Reflection * reflection, Message * message, const FieldDescriptor * field, const std::string val){ reflection->AddString(message, field, val); }, v);
}

TxStream & TxStream::operator<<(const std::vector<std::u16string> v)
{
  _field = _message_descs.top()->FindFieldByNumber(_counters.top());
  
  if (_field->cpp_type() == FieldDescriptor::CppType::CPPTYPE_STRING)
  {
    for (size_t i = 0; i < v.size(); ++i)
    {
      _reflections.top()->AddString(_mutable_msgs.top(), _field, toUTF8(v[i]));
    }
  }
  _counters.top()++;
  return *this;
}

TxStream & TxStream::operator<<(const std::vector<bool> v)
{
  return encode_vector<bool>(
    [](const Reflection * reflection, Message * message, const FieldDescriptor * field, const bool val){ reflection->AddBool(message, field, val); }, v);
}

std::string TxStream::toUTF8(const std::u16string source)
{
    std::string result;

    std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> convertor;
    result = convertor.to_bytes(source);

    return result;
}

void TxStream::load_submessage(std::string submessage_name, int index)
{
  _field = _message_descs.top()->FindFieldByNumber(_counters.top());
  _counters.top()++;
  
  if (_field->cpp_type() == FieldDescriptor::CppType::CPPTYPE_MESSAGE)
  {
    _reflections.top()->ClearField(_mutable_msgs.top(), _field);
    _mutable_msgs.push(_reflections.top()->AddMessage(_mutable_msgs.top(), _field));
    _mutable_msgs.top()->Clear();
    
    _message_descs.push(_file_desc->FindMessageTypeByName(submessage_name));
    _reflections.push(_mutable_msgs.top()->GetReflection());
  }
  
  _counters.push(1);
}

void TxStream::load_submessage(std::string submessage_name)
{
  _field = _message_descs.top()->FindFieldByNumber(_counters.top());
  
  if (_field->cpp_type() == FieldDescriptor::CppType::CPPTYPE_MESSAGE)
  {
    _mutable_msgs.push(_reflections.top()->MutableMessage(_mutable_msgs.top(), _field));
    _mutable_msgs.top()->Clear();
    
    _message_descs.push(_file_desc->FindMessageTypeByName(submessage_name));
    _reflections.push(_mutable_msgs.top()->GetReflection());
  }
  
  _counters.push(1);
}

void TxStream::unload_submessage(bool array_finished)
{
  _message_descs.pop();
  _counters.pop();
  _reflections.pop();
  
  if (array_finished)
  {
    _counters.top()++;
  }
  
  _mutable_msgs.pop();
}


// RX stream

RxStream::RxStream(uint8_t stream_type, std::string stream_name, uint8_t stream_identifier, std::string proto)
      : DynamicStream(proto)
      , _stream_type(stream_type)
      , _stream_name(stream_name)
      , _stream_identifier(stream_identifier)
      , _encoded_bytes("")
{
}

std::mutex RxStream::_rx_mutex;

bool RxStream::data_available()
{
  return _encoded_bytes.length() > 0;
}

void RxStream::clear_buffer()
{
  _encoded_bytes = "";
}

RxStream & RxStream::operator>>(uint64_t & n)
{
  if (_fields.top().end() > _field_iterators.top() && (*_field_iterators.top())->cpp_type() == FieldDescriptor::CppType::CPPTYPE_UINT64)
    n = _reflections.top()->GetUInt64(*_mutable_msgs.top(), *_field_iterators.top());

  _field_iterators.top()++;
  return *this;
}

RxStream & RxStream::operator>>(uint32_t & n)
{
  if (_fields.top().end() > _field_iterators.top() && (*_field_iterators.top())->cpp_type() == FieldDescriptor::CppType::CPPTYPE_UINT32)
    n = _reflections.top()->GetUInt32(*_mutable_msgs.top(), *_field_iterators.top());

  _field_iterators.top()++;
  return *this;
}

RxStream & RxStream::operator>>(uint16_t & n)
{
  if (_fields.top().end() > _field_iterators.top() && (*_field_iterators.top())->cpp_type() == FieldDescriptor::CppType::CPPTYPE_UINT32)
    n = static_cast<uint16_t>(_reflections.top()->GetUInt32(*_mutable_msgs.top(), *_field_iterators.top()));

  _field_iterators.top()++;
  return *this;
}

RxStream & RxStream::operator>>(uint8_t & n)
{
  if (_fields.top().end() > _field_iterators.top() && (*_field_iterators.top())->cpp_type() == FieldDescriptor::CppType::CPPTYPE_UINT32)
    n = static_cast<uint8_t>(_reflections.top()->GetUInt32(*_mutable_msgs.top(), *_field_iterators.top()));

  _field_iterators.top()++;
  return *this;
}

RxStream & RxStream::operator>>(int64_t & n)
{
  if (_fields.top().end() > _field_iterators.top() && (*_field_iterators.top())->cpp_type() == FieldDescriptor::CppType::CPPTYPE_INT64)
    n = _reflections.top()->GetInt64(*_mutable_msgs.top(), *_field_iterators.top());

  _field_iterators.top()++;
  return *this;
}

RxStream & RxStream::operator>>(int32_t & n)
{
  if (_fields.top().end() > _field_iterators.top() && (*_field_iterators.top())->cpp_type() == FieldDescriptor::CppType::CPPTYPE_INT32)
    n = _reflections.top()->GetInt32(*_mutable_msgs.top(), *_field_iterators.top());

  _field_iterators.top()++;
  return *this;
}

RxStream & RxStream::operator>>(int16_t & n)
{
  if (_fields.top().end() > _field_iterators.top() && (*_field_iterators.top())->cpp_type() == FieldDescriptor::CppType::CPPTYPE_INT32)
    n = static_cast<int16_t>(_reflections.top()->GetInt32(*_mutable_msgs.top(), *_field_iterators.top()));

  _field_iterators.top()++;
  return *this;
}

RxStream & RxStream::operator>>(int8_t & n)
{
  if (_fields.top().end() > _field_iterators.top() && (*_field_iterators.top())->cpp_type() == FieldDescriptor::CppType::CPPTYPE_INT32)
    n = static_cast<int8_t>(_reflections.top()->GetInt32(*_mutable_msgs.top(), *_field_iterators.top()));

  _field_iterators.top()++;
  return *this;
}

RxStream & RxStream::operator>>(char & n)
{
  if (_fields.top().end() > _field_iterators.top() && (*_field_iterators.top())->cpp_type() == FieldDescriptor::CppType::CPPTYPE_UINT32)
    n = static_cast<char>(_reflections.top()->GetUInt32(*_mutable_msgs.top(), *_field_iterators.top()));

  _field_iterators.top()++;
  return *this;
}

RxStream & RxStream::operator>>(float & f)
{
  return decode_field<float>(
    [](const Reflection * reflection, Message * message, const FieldDescriptor * field, float & f){ f = reflection->GetFloat(*message, field); }, f);
}

RxStream & RxStream::operator>>(double & d)
{
  return decode_field<double>(
    [](const Reflection * reflection, Message * message, const FieldDescriptor * field, double & d){ d = reflection->GetDouble(*message, field); }, d);
}

RxStream & RxStream::operator>>(std::string & s)
{
  return decode_field<std::string>(
    [](const Reflection * reflection, Message * message, const FieldDescriptor * field, std::string & s){ s = reflection->GetString(*message, field); }, s);
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
  if (_fields.top().end() > _field_iterators.top() && (*_field_iterators.top())->cpp_type() == FieldDescriptor::CppType::CPPTYPE_BOOL)
    b = _reflections.top()->GetBool(*_mutable_msgs.top(), *_field_iterators.top());

  _field_iterators.top()++;
  return *this;
}

RxStream & RxStream::operator>>(std::vector<uint64_t> & v)
{
  if (_fields.top().end() > _field_iterators.top() && (*_field_iterators.top())->cpp_type() == FieldDescriptor::CppType::CPPTYPE_UINT64)
  {
    std::vector<uint64_t> vector;
    for (int i=0; i < _reflections.top()->FieldSize(*_mutable_msgs.top(), *_field_iterators.top()); i++)
    {
      vector.push_back(_reflections.top()->GetRepeatedDouble(*_mutable_msgs.top(), *_field_iterators.top(), i));
    }
    v = vector;
  }

  _field_iterators.top()++;
  return *this;
}

RxStream & RxStream::operator>>(std::vector<uint32_t> & v)
{
  if (_fields.top().end() > _field_iterators.top() && (*_field_iterators.top())->cpp_type() == FieldDescriptor::CppType::CPPTYPE_UINT32)
  {
    std::vector<uint32_t> vector;
    for (int i=0; i < _reflections.top()->FieldSize(*_mutable_msgs.top(), *_field_iterators.top()); i++)
    {
      vector.push_back(_reflections.top()->GetRepeatedDouble(*_mutable_msgs.top(), *_field_iterators.top(), i));
    }
    v = vector;
  }

  _field_iterators.top()++;
  return *this;
}

RxStream & RxStream::operator>>(std::vector<uint16_t> & v)
{
  if (_fields.top().end() > _field_iterators.top() && (*_field_iterators.top())->cpp_type() == FieldDescriptor::CppType::CPPTYPE_UINT32)
  {
    std::vector<uint16_t> vector;
    for (int i=0; i < _reflections.top()->FieldSize(*_mutable_msgs.top(), *_field_iterators.top()); i++)
    {
      vector.push_back(static_cast<uint16_t>(_reflections.top()->GetRepeatedUInt32(*_mutable_msgs.top(), *_field_iterators.top(), i)));
    }
    v = vector;
  }

  _field_iterators.top()++;
  return *this;
}

RxStream & RxStream::operator>>(std::vector<uint8_t> & v)
{
  if (_fields.top().end() > _field_iterators.top() && (*_field_iterators.top())->cpp_type() == FieldDescriptor::CppType::CPPTYPE_UINT32)
  {
    std::vector<uint8_t> vector;
    for (int i=0; i < _reflections.top()->FieldSize(*_mutable_msgs.top(), *_field_iterators.top()); i++)
    {
      vector.push_back(static_cast<uint8_t>(_reflections.top()->GetRepeatedUInt32(*_mutable_msgs.top(), *_field_iterators.top(), i)));
    }
    v = vector;
  }

  _field_iterators.top()++;
  return *this;
}

RxStream & RxStream::operator>>(std::vector<int64_t> & v)
{
  if (_fields.top().end() > _field_iterators.top() && (*_field_iterators.top())->cpp_type() == FieldDescriptor::CppType::CPPTYPE_INT64)
  {
    std::vector<int64_t> vector;
    for (int i=0; i < _reflections.top()->FieldSize(*_mutable_msgs.top(), *_field_iterators.top()); i++)
    {
      vector.push_back(_reflections.top()->GetRepeatedInt64(*_mutable_msgs.top(), *_field_iterators.top(), i));
    }
    v = vector;
  }

  _field_iterators.top()++;
  return *this;
}

RxStream & RxStream::operator>>(std::vector<int32_t> & v)
{
  if (_fields.top().end() > _field_iterators.top() && (*_field_iterators.top())->cpp_type() == FieldDescriptor::CppType::CPPTYPE_INT32)
  {
    std::vector<int32_t> vector;
    for (int i=0; i < _reflections.top()->FieldSize(*_mutable_msgs.top(), *_field_iterators.top()); i++)
    {
      vector.push_back(_reflections.top()->GetRepeatedInt32(*_mutable_msgs.top(), *_field_iterators.top(), i));
    }
    v = vector;
  }

  _field_iterators.top()++;
  return *this;
}

RxStream & RxStream::operator>>(std::vector<int16_t> & v)
{
  if (_fields.top().end() > _field_iterators.top() && (*_field_iterators.top())->cpp_type() == FieldDescriptor::CppType::CPPTYPE_UINT32)
  {
    std::vector<int16_t> vector;
    for (int i=0; i < _reflections.top()->FieldSize(*_mutable_msgs.top(), *_field_iterators.top()); i++)
    {
      vector.push_back(static_cast<int16_t>(_reflections.top()->GetRepeatedInt32(*_mutable_msgs.top(), *_field_iterators.top(), i)));
    }
    v = vector;
  }

  _field_iterators.top()++;
  return *this;
}

RxStream & RxStream::operator>>(std::vector<int8_t> & v)
{
  if (_fields.top().end() > _field_iterators.top() && (*_field_iterators.top())->cpp_type() == FieldDescriptor::CppType::CPPTYPE_UINT32)
  {
    std::vector<int8_t> vector;
    for (int i=0; i < _reflections.top()->FieldSize(*_mutable_msgs.top(), *_field_iterators.top()); i++)
    {
      vector.push_back(static_cast<int8_t>(_reflections.top()->GetRepeatedInt32(*_mutable_msgs.top(), *_field_iterators.top(), i)));
    }
    v = vector;
  }

  _field_iterators.top()++;
  return *this;
}

RxStream & RxStream::operator>>(std::vector<char> & v)
{
  if (_fields.top().end() > _field_iterators.top() && (*_field_iterators.top())->cpp_type() == FieldDescriptor::CppType::CPPTYPE_UINT32)
  {
    std::vector<char> vector;
    for (int i=0; i < _reflections.top()->FieldSize(*_mutable_msgs.top(), *_field_iterators.top()); i++)
    {
      vector.push_back(static_cast<char>(_reflections.top()->GetRepeatedUInt32(*_mutable_msgs.top(), *_field_iterators.top(), i)));
    }
    v = vector;
  }

  _field_iterators.top()++;
  return *this;
}

RxStream & RxStream::operator>>(std::vector<float> & v)
{
  if (_fields.top().end() > _field_iterators.top() && (*_field_iterators.top())->cpp_type() == FieldDescriptor::CppType::CPPTYPE_FLOAT)
  {
    std::vector<float> vector;
    for (int i=0; i < _reflections.top()->FieldSize(*_mutable_msgs.top(), *_field_iterators.top()); i++)
    {
      vector.push_back(_reflections.top()->GetRepeatedFloat(*_mutable_msgs.top(), *_field_iterators.top(), i));
    }
    v = vector;
  }

  _field_iterators.top()++;
  return *this;
}

RxStream & RxStream::operator>>(std::vector<double> & v)
{
  if (_fields.top().end() > _field_iterators.top() && (*_field_iterators.top())->cpp_type() == FieldDescriptor::CppType::CPPTYPE_DOUBLE)
  {
    std::vector<double> vector;
    for (int i=0; i < _reflections.top()->FieldSize(*_mutable_msgs.top(), *_field_iterators.top()); i++)
    {
      vector.push_back(_reflections.top()->GetRepeatedDouble(*_mutable_msgs.top(), *_field_iterators.top(), i));
    }
    v = vector;
  }

  _field_iterators.top()++;
  return *this;
}

RxStream & RxStream::operator>>(std::vector<std::string> & v)
{
  if (_fields.top().end() > _field_iterators.top() && (*_field_iterators.top())->cpp_type() == FieldDescriptor::CppType::CPPTYPE_STRING)
  {
    std::vector<std::string> vector;
    for (int i=0; i < _reflections.top()->FieldSize(*_mutable_msgs.top(), *_field_iterators.top()); i++)
    {
      vector.push_back(_reflections.top()->GetRepeatedString(*_mutable_msgs.top(), *_field_iterators.top(), i));
    }
    v = vector;
  }

  _field_iterators.top()++;
  return *this;
}

RxStream & RxStream::operator>>(std::vector<std::u16string> & v)
{
  if (_fields.top().end() > _field_iterators.top() && (*_field_iterators.top())->cpp_type() == FieldDescriptor::CppType::CPPTYPE_STRING)
  {
    std::vector<std::u16string> vector;
    for (int i=0; i < _reflections.top()->FieldSize(*_mutable_msgs.top(), *_field_iterators.top()); i++)
    {
      vector.push_back(toUTF16(_reflections.top()->GetRepeatedString(*_mutable_msgs.top(), *_field_iterators.top(), i)));
    }
    v = vector;
  }

  _field_iterators.top()++;
  return *this;
}

RxStream & RxStream::operator>>(std::vector<bool> & v)
{
  if (_fields.top().end() > _field_iterators.top() && (*_field_iterators.top())->cpp_type() == FieldDescriptor::CppType::CPPTYPE_BOOL)
  {
    std::vector<bool> vector;
    for (int i=0; i < _reflections.top()->FieldSize(*_mutable_msgs.top(), *_field_iterators.top()); i++)
    {
      vector.push_back(_reflections.top()->GetRepeatedBool(*_mutable_msgs.top(), *_field_iterators.top(), i));
    }
    v = vector;
  }

  _field_iterators.top()++;
  return *this;
}

int RxStream::get_field_size()
{
  return _reflections.top()->FieldSize(*_mutable_msgs.top(), *_field_iterators.top());
}

void RxStream::load_submessage(std::string submessage_name, int index)
{
  if (_fields.top().end() > _field_iterators.top() && (*_field_iterators.top())->cpp_type() == FieldDescriptor::CppType::CPPTYPE_MESSAGE)
  {
    Message * msg = _reflections.top()->GetRepeatedMessage(*_mutable_msgs.top(), *_field_iterators.top(), index).New();
    msg->CopyFrom(_reflections.top()->GetRepeatedMessage(*_mutable_msgs.top(), *_field_iterators.top(), index));
    _mutable_msgs.push(msg);
    
    _message_descs.push(_file_desc->FindMessageTypeByName(submessage_name));
    
    _reflections.push(msg->GetReflection());
    
    std::vector<const FieldDescriptor*> empty_fields;
    _fields.push(empty_fields);
    
    _reflections.top()->ListFields(*msg, &_fields.top());
  
    _field_iterators.push(_fields.top().begin());
  }
}

void RxStream::load_submessage(std::string submessage_name)
{
  if (_fields.top().end() > _field_iterators.top() && (*_field_iterators.top())->cpp_type() == FieldDescriptor::CppType::CPPTYPE_MESSAGE)
  {
    Message * msg = _reflections.top()->GetMessage(*_mutable_msgs.top(), *_field_iterators.top()).New();
    msg->CopyFrom(_reflections.top()->GetMessage(*_mutable_msgs.top(), *_field_iterators.top()));
    _mutable_msgs.push(msg);
    
    _message_descs.push(_file_desc->FindMessageTypeByName(submessage_name));
    
    _reflections.push(msg->GetReflection());
    
    std::vector<const FieldDescriptor*> empty_fields;
    _fields.push(empty_fields);
    
    _reflections.top()->ListFields(*msg, &_fields.top());
    
    _field_iterators.push(_fields.top().begin());
  }
  
}

void RxStream::unload_submessage(bool array_finished)
{
  if (_message_descs.size() > 1)
  {
    _mutable_msgs.pop();
    _message_descs.pop();
    _reflections.pop();
    
    _fields.pop();
    _field_iterators.pop();
    
    if (array_finished)
    {
      _field_iterators.top()++;
    }
  }
}

void RxStream::interpret_packets(int64_t wanted_sequence_id)
{
  if (data_available()) return;

  std::lock_guard<std::mutex> lock(_rx_mutex);
  
  DynamicMessage::_dynamic_descriptor = _message_descs.top(); // Segfault on services because _message_descs was getting empty
  _codec.load<DynamicMessage>();
  
  std::queue<std::vector<uint8_t>> & packets = TcpDaemon::read_packets();
  
  for (int i = 0; i < packets.size(); i++)
  {
    std::vector<uint8_t> packet;
    
    if (!packets.empty())
    {
      packet = packets.front();
      packets.pop();
    }
    else
    {
      break;
    }
    
    std::string encoded_bytes(packet.begin(), packet.end());
    
    // TODO Il rebuffer packet è insensato in quanto se viene invocato genera un numero infinito di iterazioni sui pacchetti in rx
    // TODO Inoltre il pacchetto viene sempre prelevato, anche se l'id non matcha, così le altre istanze non lo leggeranno mai
    
    // Check the topic name with its id
    if(_codec.id(encoded_bytes) == _codec.id<DynamicMessage>())
    {
      uint8_t stream_type;
      int64_t sequence_id;
      
      _mutable_msgs.top()->Clear();
      
      try
      {
        _codec.decode(encoded_bytes, _mutable_msgs.top());
      }
      catch (...)
      {
        packets.push(packet);
        continue;
      }
      
      std::vector<const FieldDescriptor*> empty_fields;
      _fields.push(empty_fields);
      
      _reflections.top()->ListFields(*_mutable_msgs.top(), &_fields.top());
      _field_iterators.push(_fields.top().begin());

      if (*_field_iterators.top())
      {
        stream_type = _reflections.top()->GetUInt32(*_mutable_msgs.top(), *_field_iterators.top());
        
        if (_stream_type != _stream_type_match_map.at(stream_type))
        {
          packets.push(packet);
          continue;
        }
      }
      
      _field_iterators.top()++;
      
      if (*_field_iterators.top() && _stream_type == CLIENT_TYPE)
      {
        sequence_id = _reflections.top()->GetUInt64(*_mutable_msgs.top(), *_field_iterators.top());
        
        if (sequence_id != wanted_sequence_id || stream_type != SERVICE_TYPE)
        {
          packets.push(packet);
          continue;
        }
      
        _field_iterators.top()++;
      }
      
      // If the packet matches load the current bytes in the corresponding private member
      _encoded_bytes = encoded_bytes;
      
      DCCL_DEBUG("RX incoming:\n%s\n", _mutable_msgs.top()->DebugString().c_str());
      
      break;
    }
    else
    {
      packets.push(packet);
    }
  }
}

std::u16string RxStream::toUTF16(const std::string source)
{
    std::u16string result;

    std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> convertor;
    result = convertor.from_bytes(source);

    return result;
}


}

