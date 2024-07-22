#ifndef MACROS_HPP_
#define MACROS_HPP_

#include <limits>
#include <string>

#include "rosidl_runtime_c/primitives_sequence.h"
#include "rosidl_runtime_c/primitives_sequence_functions.h"

#define SPECIALIZE_GENERIC_C_SEQUENCE(C_NAME, C_TYPE) \
  template<> \
  struct GenericCSequence<C_TYPE> \
  { \
    using type = rosidl_runtime_c__ ## C_NAME ## __Sequence; \
 \
    static void fini(type * sequence) { \
      rosidl_runtime_c__ ## C_NAME ## __Sequence__fini(sequence); \
    } \
 \
    static bool init(type * sequence, size_t size) { \
      return rosidl_runtime_c__ ## C_NAME ## __Sequence__init(sequence, size); \
    } \
  };

template<typename T>
struct GenericCSequence;

// multiple definitions of ambiguous primitive types
SPECIALIZE_GENERIC_C_SEQUENCE(bool, bool)
SPECIALIZE_GENERIC_C_SEQUENCE(byte, uint8_t)
SPECIALIZE_GENERIC_C_SEQUENCE(char, char)
SPECIALIZE_GENERIC_C_SEQUENCE(float32, float)
SPECIALIZE_GENERIC_C_SEQUENCE(float64, double)
SPECIALIZE_GENERIC_C_SEQUENCE(int8, int8_t)
SPECIALIZE_GENERIC_C_SEQUENCE(int16, int16_t)
SPECIALIZE_GENERIC_C_SEQUENCE(uint16, uint16_t)
SPECIALIZE_GENERIC_C_SEQUENCE(int32, int32_t)
SPECIALIZE_GENERIC_C_SEQUENCE(uint32, uint32_t)
SPECIALIZE_GENERIC_C_SEQUENCE(int64, int64_t)
SPECIALIZE_GENERIC_C_SEQUENCE(uint64, uint64_t)

#endif  // MACROS_HPP_
