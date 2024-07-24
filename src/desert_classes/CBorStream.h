#ifndef CBORSTREAM_H_
#define CBORSTREAM_H_

#include "TcpDaemon.h"

#include <map>
#include <queue>
#include <utility>
#include <vector>
#include <string>
#include <locale>
#include <codecvt>
#include <cstdint>
#include <cstdio>

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

#define MAX_PACKET_LENGTH 512

namespace cbor
{

class TxStream
{
  public:
    TxStream(uint8_t stream_type, std::string stream_name);
    
    void start_transmission(uint64_t sequence_id);
    void start_transmission();
    void end_transmission();

    TxStream & operator<<(const uint64_t n);
    TxStream & operator<<(const uint32_t n);
    TxStream & operator<<(const uint16_t n);
    TxStream & operator<<(const uint8_t n);
    TxStream & operator<<(const int64_t n);
    TxStream & operator<<(const int32_t n);
    TxStream & operator<<(const int16_t n);
    TxStream & operator<<(const int8_t n);
    TxStream & operator<<(const char n);
    TxStream & operator<<(const float f);
    TxStream & operator<<(const double d);
    TxStream & operator<<(const std::string s);
    TxStream & operator<<(const std::u16string s);
    TxStream & operator<<(const bool b);
    
    template<typename T>
    inline TxStream & operator<<(const std::vector<T> v)
    {
      *this << static_cast<const uint32_t>(v.size());
      return serialize_sequence(v.data(), v.size());
    }
    
    TxStream & operator<<(const std::vector<bool> v);
    
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
    RxStream(uint8_t stream_type, std::string stream_name);
    
    bool data_available(uint64_t sequence_id = 0);
    
    RxStream & operator>>(uint64_t & n);
    RxStream & operator>>(uint32_t & n);
    RxStream & operator>>(uint16_t & n);
    RxStream & operator>>(uint8_t & n);
    RxStream & operator>>(int64_t & n);
    RxStream & operator>>(int32_t & n);
    RxStream & operator>>(int16_t & n);
    RxStream & operator>>(int8_t & n);
    
    template<typename T>
    RxStream & deserialize_integer(T & n);
    
    RxStream & operator>>(char & n);
    RxStream & operator>>(float & f);
    RxStream & operator>>(double & d);
    RxStream & operator>>(std::string & s);
    RxStream & operator>>(std::u16string & s);
    RxStream & operator>>(bool & b);
    
    template<typename T>
    inline RxStream & operator>>(std::vector<T> & v)
    {
      uint32_t size;
      *this >> size;
      v.resize(size);
      
      return deserialize_sequence(v.data(), size);
    }
    
    RxStream & operator>>(std::vector<bool> & v);
    
    template<typename T>
    inline RxStream & deserialize_sequence(T * items, size_t size)
    {
      for (size_t i = 0; i < size; ++i)
      {
        *this >> items[i];
      }
      return *this;
    }
    
    static void interpret_packets();
  
  private:
    uint8_t _stream_type;
    std::string _stream_name;
    
    int _buffered_iterator;
    std::vector<std::pair<void *, int>> _buffered_packet;
    
    // <topic, packets <packet <field, field_type>>>
    static std::map<std::string, std::queue<std::vector<std::pair<void *, int>>>> _interpreted_publications;
    // <service, packets <packet <field, field_type>>>
    static std::map<std::string, std::queue<std::vector<std::pair<void *, int>>>> _interpreted_requests;
    // <service + id, packets <packet <field, field_type>>>
    static std::map<std::string, std::queue<std::vector<std::pair<void *, int>>>> _interpreted_responses;
    
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
    
    static std::pair<void *, int> interpret_field(cbor_item_t * items, size_t i, union _cbor_value & val);
    std::u16string toUTF16(const std::string source);
};

}  // namespace cbor


#endif
