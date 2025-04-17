/****************************************************************************
 * Copyright (C) 2024 Davide Costa                                          *
 *                                                                          *
 * This file is part of RMW desert.                                         *
 *                                                                          *
 *   RMW desert is free software: you can redistribute it and/or modify it  *
 *   under the terms of the GNU General Public License as published by the  *
 *   Free Software Foundation, either version 3 of the License, or any      *
 *   later version.                                                         *
 *                                                                          *
 *   RMW desert is distributed in the hope that it will be useful,          *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of         *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          *
 *   GNU General Public License for more details.                           *
 *                                                                          *
 *   You should have received a copy of the GNU General Public License      *
 *   along with RMW desert.  If not, see <http://www.gnu.org/licenses/>.    *
 ****************************************************************************/

/**
 * @file DcclStream.h
 * @brief Classes used to convert data types into a DCCL encoded stream
 * 
 * In order to perform a socket communication different data types needs to be 
 * encoded into binary representations so they can be sent through the same channel. 
 * DCCL fits perfectly with the DESERT requirements because only a minimal overhead
 * is introduced in the stream and all the data types are sent using only the minimal
 * quantity of bytes possible.
 *
 * @author Prof. Davide Costa
 *
 */

#ifndef DCCLSTREAM_H_
#define DCCLSTREAM_H_

#include "TcpDaemon.h"
#include "TopicsConfig.h"

/** @cond */

#include <map>
#include <queue>
#include <utility>
#include <vector>
#include <string>
#include <locale>
#include <codecvt>
#include <cstdint>
#include <cstdio>
#include <mutex>
#include <bitset>
#include <stack>

#include <google/protobuf/descriptor.h>
#include <google/protobuf/dynamic_message.h>
#include <google/protobuf/io/zero_copy_stream_impl.h>
#include <google/protobuf/io/tokenizer.h>
#include <google/protobuf/compiler/importer.h>
#include <google/protobuf/compiler/parser.h>

#include "rmw/types.h"

/** @endcond */
 
#include "dccl.h"
#include "dccl_import.pb.h"

#define PUBLISHER_TYPE  0
#define SUBSCRIBER_TYPE 1
#define CLIENT_TYPE     2
#define SERVICE_TYPE    3

#define DCCL_DEBUG_ENABLED 0

#if DCCL_DEBUG_ENABLED
#define DCCL_DEBUG(...) printf(__VA_ARGS__)
#else
#define DCCL_DEBUG(...) 
#endif

#define MAX_BUFFER_CAPACITY 100
/*
template <typename T, int MaxLen, typename Container=std::deque<T>>
class CircularQueue : public std::queue<T, Container> {
  public:
    void push(const T& value)
    {
        if (this->size() == MaxLen)
        {
           this->c.pop_front();
        }
        std::queue<T, Container>::push(value);
    }
};
*/
using namespace google::protobuf;
using namespace compiler;
using namespace io;

namespace dccl
{

class DynamicMessage final : public Message
{
  public:
    static const Descriptor * descriptor()
    {
        return _dynamic_descriptor;
    }
    static const Descriptor * _dynamic_descriptor;
};

class DynamicStream
{
  public:
    DynamicStream(std::string protobuf);
    
  private:
    rmw_ret_t get_message_descriptor();
    rmw_ret_t get_mutable_message();
    rmw_ret_t get_reflection();
    
    std::string _proto;

  protected:
    FileDescriptorProto _file_desc_proto;
    DescriptorPool _pool;
    const FileDescriptor * _file_desc;

    std::stack<const Descriptor *> _message_descs;
    DynamicMessageFactory _factory;
    const Message * _prototype_msg;
    std::stack<Message *> _mutable_msgs;
    std::stack<const Reflection *> _reflections;
    
    rmw_ret_t _internal_state;
    static const std::map<std::type_index, int> _types_map;
    static const std::map<int, int> _stream_type_match_map;
    
    dccl::Codec _codec;
    
};

class TxStream : public DynamicStream
{
  public:
   /**
    * @brief Create a transmission stream
    *
    * @param stream_type        Type of the object using the current instance
    * @param stream_name        Name of the topic or the service to which the communication belongs
    * @param stream_identifier  Identifier of the topic or the service read from configuration
    * @param proto              TODO
    */
    TxStream(uint8_t stream_type, std::string stream_name, uint8_t stream_identifier, std::string proto);
    
   /**
    * @brief Tell the stream to create a new packet
    *
    * Every time a transmission in started, _counter must be set to one.
    * Then type, service name and sequence id are put in front of the data.
    *
    * @param sequence_id The id of the client service communication
    */
    void start_transmission(uint64_t sequence_id);
   /**
    * @brief Tell the stream to create a new packet
    *
    * Every time a transmission in started, _counter must be set to one.
    * Then type and topic name are put in front of the data.
    */
    void start_transmission();
   /**
    * @brief Tell the stream to send down the packet
    *
    * When the transmission is finished the packet is stored in the 
    * static member of TcpDaemon in order to be sent to DESERT.
    */
    void end_transmission();

   /**
    * @brief Encode uint64
    * @param n Field to encode
    */
    TxStream & operator<<(const uint64_t n);
   /**
    * @brief Encode uint32
    * @param n Field to encode
    */
    TxStream & operator<<(const uint32_t n);
   /**
    * @brief Encode uint16
    * @param n Field to encode
    */
    TxStream & operator<<(const uint16_t n);
   /**
    * @brief Encode uint8
    * @param n Field to encode
    */
    TxStream & operator<<(const uint8_t n);
   /**
    * @brief Encode int64
    * @param n Field to encode
    */
    TxStream & operator<<(const int64_t n);
   /**
    * @brief Encode int32
    * @param n Field to encode
    */
    TxStream & operator<<(const int32_t n);
   /**
    * @brief Encode int16
    * @param n Field to encode
    */
    TxStream & operator<<(const int16_t n);
   /**
    * @brief Encode int8
    * @param n Field to encode
    */
    TxStream & operator<<(const int8_t n);
   /**
    * @brief Encode char
    * @param n Field to encode
    */
    TxStream & operator<<(const char n);
   /**
    * @brief Encode float
    * @param f Field to encode
    */
    TxStream & operator<<(const float f);
   /**
    * @brief Encode double
    * @param d Field to encode
    */
    TxStream & operator<<(const double d);
   /**
    * @brief Encode string
    * @param s Field to encode
    */
    TxStream & operator<<(const std::string s);
   /**
    * @brief Encode u16string
    * @param s Field to encode
    */
    TxStream & operator<<(const std::u16string s);
   /**
    * @brief Encode bool
    * @param b Field to encode
    */
    TxStream & operator<<(const bool b);
    
    template<typename T>
    inline TxStream & encode_field(std::function<void(const Reflection *, Message *, const FieldDescriptor *, const T)> add_value, const T t)
    {
      if (_internal_state != RMW_RET_OK)
        return *this;
      
      _field = _message_descs.top()->FindFieldByNumber(_counters.top());
      
      if (_field == nullptr)
      {
        _counters.top()++;
        return *this;
      }
        
      if (_field->cpp_type() == _types_map.at(typeid(T)))
        add_value(_reflections.top(), _mutable_msgs.top(), _field, t);
      
      _counters.top()++;
      return *this;
    }
    
   /**
    * @brief Encode uint64 vector
    * @param v Field to encode
    */
    TxStream & operator<<(const std::vector<uint64_t> v);
   /**
    * @brief Encode uint32 vector
    * @param v Field to encode
    */
    TxStream & operator<<(const std::vector<uint32_t> v);
   /**
    * @brief Encode uint16 vector
    * @param v Field to encode
    */
    TxStream & operator<<(const std::vector<uint16_t> v);
   /**
    * @brief Encode uint8 vector
    * @param v Field to encode
    */
    TxStream & operator<<(const std::vector<uint8_t> v);
   /**
    * @brief Encode int64 vector
    * @param v Field to encode
    */
    TxStream & operator<<(const std::vector<int64_t> v);
   /**
    * @brief Encode int32 vector
    * @param v Field to encode
    */
    TxStream & operator<<(const std::vector<int32_t> v);
   /**
    * @brief Encode int16 vector
    * @param v Field to encode
    */
    TxStream & operator<<(const std::vector<int16_t> v);
   /**
    * @brief Encode int8 vector
    * @param v Field to encode
    */
    TxStream & operator<<(const std::vector<int8_t> v);
   /**
    * @brief Encode char vector
    * @param v Field to encode
    */
    TxStream & operator<<(const std::vector<char> v);
   /**
    * @brief Encode float vector
    * @param v Field to encode
    */
    TxStream & operator<<(const std::vector<float> v);
   /**
    * @brief Encode double vector
    * @param v Field to encode
    */
    TxStream & operator<<(const std::vector<double> v);
   /**
    * @brief Encode string vector
    * @param v Field to encode
    */
    TxStream & operator<<(const std::vector<std::string> v);
   /**
    * @brief Encode u16string vector
    * @param v Field to encode
    */
    TxStream & operator<<(const std::vector<std::u16string> v);
   /**
    * @brief Encode bool vector
    * @param v Field to encode
    */
    TxStream & operator<<(const std::vector<bool> v);
    
    template<typename T>
    inline TxStream & encode_vector(std::function<void(const Reflection *, Message *, const FieldDescriptor *, const T)> add_value, const std::vector<T> v)
    {
      if (_internal_state != RMW_RET_OK)
        return *this;
      
      _field = _message_descs.top()->FindFieldByNumber(_counters.top());

      if (_field == nullptr)
      {
        _counters.top()++;
        return *this;
      }

      if (_field->cpp_type() == _types_map.at(typeid(T)))
      {
        _reflections.top()->ClearField(_mutable_msgs.top(), _field);
        for (size_t i = 0; i < v.size(); ++i)
        {
          add_value(_reflections.top(), _mutable_msgs.top(), _field, v[i]);
        }
      }
      _counters.top()++;
      return *this;
    }
    
    template<typename T>
    inline TxStream & cast_encode_vector(std::function<void(const Reflection *, Message *, const FieldDescriptor *, const T)> add_value, const std::vector<T> v)
    {
      if (_internal_state != RMW_RET_OK)
        return *this;
      
      _field = _message_descs.top()->FindFieldByNumber(_counters.top());
      
      if (_field == nullptr)
      {
        _counters.top()++;
        return *this;
      }
        
      if (_field->cpp_type() == _types_map.at(typeid(int32_t)))
      {
        _reflections.top()->ClearField(_mutable_msgs.top(), _field);
        for (size_t i = 0; i < v.size(); ++i)
        {
          add_value(_reflections.top(), _mutable_msgs.top(), _field, static_cast<int32_t>(v[i]));
        }
      }
      _counters.top()++;
      return *this;
    }
    
    template<typename T>
    inline TxStream & ucast_encode_vector(std::function<void(const Reflection *, Message *, const FieldDescriptor *, const T)> add_value, const std::vector<T> v)
    {
      if (_internal_state != RMW_RET_OK)
        return *this;
      
      _field = _message_descs.top()->FindFieldByNumber(_counters.top());
      
      if (_field == nullptr)
      {
        _counters.top()++;
        return *this;
      }
        
      if (_field->cpp_type() == _types_map.at(typeid(uint32_t)))
      {
        _reflections.top()->ClearField(_mutable_msgs.top(), _field);
        for (size_t i = 0; i < v.size(); ++i)
        {
          add_value(_reflections.top(), _mutable_msgs.top(), _field, static_cast<uint32_t>(v[i]));
        }
      }
      _counters.top()++;
      return *this;
    }
    
   /**
    * @brief Serialize a sequence of uniform elements
    * @param items Pointer to the first element
    * @param size Size of the items array
    */
    template<typename T>
    inline TxStream & serialize_sequence(const T * items, size_t size)
    {
      std::vector<T> vec;
      
      for (size_t i = 0; i < size; ++i)
      {
        vec.push_back(items[i]);
      }
      
      *this << vec;
      return *this;
    }
    
    void load_submessage(std::string submessage_name, int index);
    void load_submessage(std::string submessage_name);
    void unload_submessage(bool array_finished = true);

  private:
    uint8_t _stream_type;
    std::string _stream_name;
    uint8_t _stream_identifier;
    
    std::string _encoded_bytes;
    
    std::stack<int> _counters;
    const FieldDescriptor * _field;
    
    std::string toUTF8(const std::u16string source);
  
};

class RxStream : public DynamicStream
{
  public:
   /**
    * @brief Create a reception stream
    *
    * @param stream_type        Type of the object using the current instance
    * @param stream_name        Name of the topic or the service to which the communication belongs
    * @param stream_identifier  Identifier of the topic or the service read from configuration
    * @param proto              TODO
    */
    RxStream(uint8_t stream_type, std::string stream_name, uint8_t stream_identifier, std::string proto);
    
   /**
    * @brief Check if there are data
    *
    * A map contains the information received for all topics and services, 
    * so using the name saved in the current instance as key it is possible 
    * to know if a message is arrived for a specific entity.
    */
    bool data_available();
    
   /**
    * @brief Clear the currently buffered packet
    *
    * When the packet is read by the entity, this function must be called to clear the buffer 
    * and allow RxStream to add the next one in the queue.
    */
    void clear_buffer();
    
   /**
    * @brief Decode uint64
    * @param n Field to decode
    */
    RxStream & operator>>(uint64_t & n);
   /**
    * @brief Decode uint32
    * @param n Field to decode
    */
    RxStream & operator>>(uint32_t & n);
   /**
    * @brief Decode uint16
    * @param n Field to decode
    */
    RxStream & operator>>(uint16_t & n);
   /**
    * @brief Decode uint8
    * @param n Field to decode
    */
    RxStream & operator>>(uint8_t & n);
   /**
    * @brief Decode int64
    * @param n Field to decode
    */
    RxStream & operator>>(int64_t & n);
   /**
    * @brief Decode int32
    * @param n Field to decode
    */
    RxStream & operator>>(int32_t & n);
   /**
    * @brief Decode int16
    * @param n Field to decode
    */
    RxStream & operator>>(int16_t & n);
   /**
    * @brief Decode int8
    * @param n Field to decode
    */
    RxStream & operator>>(int8_t & n);
   /**
    * @brief Decode char
    * @param n Field to decode
    */
    RxStream & operator>>(char & n);
   /**
    * @brief Decode float
    * @param f Field to decode
    */
    RxStream & operator>>(float & f);
   /**
    * @brief Decode double
    * @param d Field to decode
    */
    RxStream & operator>>(double & d);
   /**
    * @brief Decode string
    * @param s Field to decode
    */
    RxStream & operator>>(std::string & s);
   /**
    * @brief Decode u16string
    * @param s Field to decode
    */
    RxStream & operator>>(std::u16string & s);
   /**
    * @brief Decode bool
    * @param b Field to decode
    */
    RxStream & operator>>(bool & b);
    
    template<typename T>
    inline RxStream & decode_field(std::function<void(const Reflection *, Message *, const FieldDescriptor *, T &)> get_value, T & t)
    {
      if (_internal_state != RMW_RET_OK)
        return *this;
      
      if (*_field_iterators.top() == nullptr)
      {
        _field_iterators.top()++;
        return *this;
      }
      
      if (_fields.top().end() > _field_iterators.top() && (*_field_iterators.top())->cpp_type() ==  _types_map.at(typeid(T)))
        get_value(_reflections.top(), _mutable_msgs.top(), *_field_iterators.top(), t);

      _field_iterators.top()++;
      return *this;
    }
      
   /**
    * @brief Decode uint64 vector
    * @param v Field to decode
    */
    RxStream & operator>>(std::vector<uint64_t> & v);
   /**
    * @brief Decode uint32 vector
    * @param v Field to decode
    */
    RxStream & operator>>(std::vector<uint32_t> & v);
   /**
    * @brief Decode uint16 vector
    * @param v Field to decode
    */
    RxStream & operator>>(std::vector<uint16_t> & v);
   /**
    * @brief Decode uint8 vector
    * @param v Field to decode
    */
    RxStream & operator>>(std::vector<uint8_t> & v);
   /**
    * @brief Decode int64 vector
    * @param v Field to decode
    */
    RxStream & operator>>(std::vector<int64_t> & v);
   /**
    * @brief Decode int32 vector
    * @param v Field to decode
    */
    RxStream & operator>>(std::vector<int32_t> & v);
   /**
    * @brief Decode int16 vector
    * @param v Field to decode
    */
    RxStream & operator>>(std::vector<int16_t> & v);
   /**
    * @brief Decode int8 vector
    * @param v Field to decode
    */
    RxStream & operator>>(std::vector<int8_t> & v);
   /**
    * @brief Decode char vector
    * @param v Field to decode
    */
    RxStream & operator>>(std::vector<char> & v);
   /**
    * @brief Decode float vector
    * @param v Field to decode
    */
    RxStream & operator>>(std::vector<float> & v);
   /**
    * @brief Decode double vector
    * @param v Field to decode
    */
    RxStream & operator>>(std::vector<double> & v);
   /**
    * @brief Decode string vector
    * @param v Field to decode
    */
    RxStream & operator>>(std::vector<std::string> & v);
   /**
    * @brief Decode u16string vector
    * @param v Field to decode
    */
    RxStream & operator>>(std::vector<std::u16string> & v);
   /**
    * @brief Decode bool vector
    * @param v Field to decode
    */
    RxStream & operator>>(std::vector<bool> & v);
    
   /**
    * @brief Deserialize a sequence of uniform elements
    * @param items Pointer to the first element
    * @param size Size of the items array
    */
    template<typename T>
    inline RxStream & deserialize_sequence(T * items, size_t size)
    {
      std::vector<T> vec(size);
      *this >> vec;
      
      for (size_t i = 0; i < size; ++i)
      {
        items[i] = vec.at(i);
      }
      
      return *this;
    }
    
    int get_field_size();
    
    void load_submessage(std::string submessage_name, int index);
    void load_submessage(std::string submessage_name);
    void unload_submessage(bool array_finished = true);
    
   /**
    * @brief Interpret raw packets
    *
    * Raw packets from TcpDaemon are read and interpreted in order to put them in 
    * a string private member.
    */
    void interpret_packets(int64_t wanted_sequence_id = 0);
  
  private:
    uint8_t _stream_type;
    std::string _stream_name;
    uint8_t _stream_identifier;
    
    std::string _encoded_bytes;
    
    std::stack<std::vector<const FieldDescriptor*>> _fields;
    std::stack<std::vector<const FieldDescriptor*>::iterator> _field_iterators;
    
    static std::mutex _rx_mutex;
    
    std::u16string toUTF16(const std::string source);
};

}  // namespace dccl


#endif
