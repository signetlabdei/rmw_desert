#ifndef CBORSTREAM_HPP_
#define CBORSTREAM_HPP_

#include <vector>
#include <string>
#include <cstdint>
#include <stdio.h>

namespace cbor
{

class TxStream
{
  public:
    TxStream(size_t hint = 1024);

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
  
};

class RxStream
{
  public:
    RxStream();
};

}  // namespace cbor


#endif
