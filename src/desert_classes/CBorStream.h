#ifndef CBORSTREAM_HPP_
#define CBORSTREAM_HPP_

#include "TcpDaemon.h"
//#include "DesertSubscriber.h"

#include <map>
#include <set>
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

#define MAX_PACKET_LENGTH 512

namespace cbor
{

class TxStream
{
  public:
    TxStream();
    
    void start_transmission(std::string topic_name);
    void end_transmission();

    TxStream & operator<<(const uint64_t n);
    TxStream & operator<<(const uint32_t n);
    TxStream & operator<<(const uint16_t n);
    TxStream & operator<<(const uint8_t n);
    TxStream & operator<<(const int64_t i);
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
    TxStream & operator<<(const std::vector<T> v);
    TxStream & operator<<(const std::vector<bool> v);
    template<typename T>
    TxStream & serialize_sequence(const T * items, size_t size);

  private:
    size_t size_;
    bool _overflow;
    int _sequence_id;
    uint8_t *  _packet;
    cbor_writer_t *  _writer;
    
    void new_packet();
    void handle_overrun(cbor_error_t result);
    
    std::string toUTF8(const std::u16string source);
  
};

class RxStream
{
  public:
    RxStream(std::string topic_name);
    
    RxStream & operator>>(const void ** data);
    
    static void defragment_packets();
    static void interpret_packets();
  
  private:
    std::string _topic_name;
    static std::map<std::string, std::vector<uint8_t>> _defragmented_packets;
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
};

}  // namespace cbor


#endif
