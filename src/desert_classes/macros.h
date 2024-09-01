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
 * @file macros.h
 * @brief Header containing C sequence macros
 * 
 * The C data type implementation is more complicated than the C++ one, because complex 
 * types like vectors have to be manually managed and this header contains definitions 
 * used to create dynamic element sequences.
 *
 * @author Prof. Davide Costa
 *
 */

#ifndef MACROS_H_
#define MACROS_H_

/** @cond */

#include "rosidl_runtime_c/primitives_sequence.h"
#include "rosidl_runtime_c/primitives_sequence_functions.h"

/** @endcond */

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
