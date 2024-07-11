#ifndef CBORSTREAM_HPP_
#define CBORSTREAM_HPP_

#include <vector>
#include <string>
#include <locale>
#include <codecvt>
#include <cstdint>
#include <stdio.h>

#include "cbor/encoder.h"
#include "cbor/ieee754.h"
#include "cbor/decoder.h"
#include "cbor/parser.h"
#include "cbor/helper.h"

#define MAX_PACKET_LENGTH 125

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
    int _sequence_id;
    std::vector<uint8_t *>  _packets;
    std::vector<cbor_writer_t *>  _writers;
    
    void add_packet();
    template<typename T>
    void handle_overrun(cbor_error_t result, T parameter);
    
    std::string toUTF8(const std::u16string source);
  
};

class RxStream
{
  public:
    RxStream();
};

}  // namespace cbor


#endif
