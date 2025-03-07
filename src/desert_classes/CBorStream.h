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
 * @file CBorStream.h
 * @brief Classes used to convert data types into a CBOR encoded stream
 * 
 * In order to perform a socket communication different data types needs to be 
 * encoded into binary representations so they can be sent through the same channel. 
 * CBOR fits perfectly with the DESERT requirements because only a minimal overhead
 * is introduced in the stream and all the data types are sent using only the minimal
 * quantity of bytes possible.
 *
 * @author Prof. Davide Costa
 *
 */

#ifndef CBORSTREAM_H_
#define CBORSTREAM_H_

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

/** @endcond */

#include "cbor/encoder.h"
#include "cbor/ieee754.h"
#include "cbor/decoder.h"
#include "cbor/parser.h"
#include "cbor/helper.h"

#include "half.hpp"

#define PUBLISHER_TYPE  0
#define SUBSCRIBER_TYPE 1
#define CLIENT_TYPE     2
#define SERVICE_TYPE    3

#define MAX_BUFFER_CAPACITY 100

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

namespace cbor
{

class TxStream
{
  public:
   /**
    * @brief Create a transmission stream
    *
    * @param stream_type        Type of the object using the current instance
    * @param stream_name        Name of the topic or the service to which the communication belongs
    * @param stream_identifier  Identifier of the topic or the service read from configuration
    */
    TxStream(uint8_t stream_type, std::string stream_name, uint8_t stream_identifier);
    
   /**
    * @brief Tell the stream to create a new packet
    *
    * Every time a transmission in started, a new empty packet must be 
    * generated and saved as a private member. Then type, service name 
    * and sequence id are put in front of the data.
    *
    * @param sequence_id The id of the client service communication
    */
    void start_transmission(uint64_t sequence_id);
   /**
    * @brief Tell the stream to create a new packet
    *
    * Every time a transmission in started, a new empty packet must be 
    * generated and saved as a private member. Then type and topic name 
    * are put in front of the data.
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
    
   /**
    * @brief Encode vector
    * @param v Field to encode
    */
    template<typename T>
    inline TxStream & operator<<(const std::vector<T> v)
    {
      *this << static_cast<const uint32_t>(v.size());
      return serialize_sequence(v.data(), v.size());
    }
    
   /**
    * @brief Encode bool vector
    * @param v Field to encode
    */
    TxStream & operator<<(const std::vector<bool> v);
    
   /**
    * @brief Serialize a sequence of uniform elements
    * @param items Pointer to the first element
    * @param size Size of the items array
    */
    template<typename T>
    inline TxStream & serialize_sequence(const T * items, size_t size)
    {
      for (size_t i = 0; i < size; ++i)
      {
        *this << items[i];
      }
      return *this;
    }

  private:
    uint8_t _stream_type;
    std::string _stream_name;
    uint8_t _stream_identifier;
    
    bool _overflow;
    uint8_t *  _packet;
    cbor_writer_t *  _writer;
    
    void new_packet();
    void handle_overrun(cbor_error_t result);
    
    std::string toUTF8(const std::u16string source);
  
};

class RxStream
{
  public:
   /**
    * @brief Create a reception stream
    *
    * @param stream_type        Type of the object using the current instance
    * @param stream_name        Name of the topic or the service to which the communication belongs
    * @param stream_identifier  Identifier of the topic or the service read from configuration
    */
    RxStream(uint8_t stream_type, std::string stream_name, uint8_t stream_identifier);
    
   /**
    * @brief Check if there are data
    *
    * A map contains the information received for all topics and services, 
    * so using the name saved in the current instance as key it is possible 
    * to know if a message is arrived for a specific entity.
    *
    * @param sequence_id The id of the client service communication
    */
    bool data_available(int64_t sequence_id = 0);
    
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
    * @brief Decode a generic integer
    * @param n Field to decode
    */
    template<typename T>
    RxStream & deserialize_integer(T & n);
    
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
    
   /**
    * @brief Decode vector
    * @param v Field to decode
    */
    template<typename T>
    inline RxStream & operator>>(std::vector<T> & v)
    {
      uint32_t size;
      *this >> size;
      v.resize(size);
      
      return deserialize_sequence(v.data(), size);
    }
    
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
      for (size_t i = 0; i < size; ++i)
      {
        *this >> items[i];
      }
      return *this;
    }
    
   /**
    * @brief Interpret raw packets and splits them into different communication types
    *
    * Raw packets from TcpDaemon are read and interpreted in order to put them in 
    * a map where the key allows to distinguish the topic name or the service name, 
    * and eventually the sequence identifier.
    */
    static void interpret_packets();
  
  private:
    uint8_t _stream_type;
    std::string _stream_name;
    uint8_t _stream_identifier;
    
    size_t _buffered_iterator;
    std::vector<std::pair<void *, int>> _buffered_packet;
    
    // <topic, packets <packet <field, field_type>>>
    static std::map<uint32_t, CircularQueue<std::vector<std::pair<void *, int>>, MAX_BUFFER_CAPACITY>> _interpreted_publications;
    // <service, packets <packet <field, field_type>>>
    static std::map<uint32_t, CircularQueue<std::vector<std::pair<void *, int>>, MAX_BUFFER_CAPACITY>> _interpreted_requests;
    // <service + id, packets <packet <field, field_type>>>
    static std::map<uint32_t, CircularQueue<std::vector<std::pair<void *, int>>, MAX_BUFFER_CAPACITY>> _interpreted_responses;
    
    union _cbor_value {
	int8_t i8;
	int16_t i16;
	int32_t i32;
	int64_t i64;
	float f32;
	double f64;
	uint8_t *bin;
	char *str;
	uint8_t str_copy[128];
    };
    
    static std::mutex _rx_mutex;
    
    static std::pair<void *, int> interpret_field(cbor_item_t * items, size_t i, union _cbor_value & val);
    std::u16string toUTF16(const std::string source);
};

}  // namespace cbor


#endif
