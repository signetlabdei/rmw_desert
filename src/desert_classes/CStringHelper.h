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
 * @file CStringHelper.h
 * @brief Namespace containing C sequence handling functions
 * 
 * The C data type implementation is more complicated than the C++ one, because complex 
 * types like vectors have to be manually managed and this header contains functions to 
 * convert C strings and generic sequences into respectively C++ strings and vectors.
 *
 * @author Prof. Davide Costa
 *
 */

#ifndef CSTRING_HELPER_H_
#define CSTRING_HELPER_H_

/** @cond */

#include "rosidl_runtime_c/u16string.h"
#include "rosidl_runtime_c/string.h"
#include "rosidl_runtime_c/u16string_functions.h"
#include "rosidl_runtime_c/string_functions.h"

#include <stdexcept>
#include <vector>
#include <string>

/** @endcond */

/**
 * @namespace CStringHelper
 * @brief Namespace containing C sequence handling functions
 * 
 * The C data type implementation is more complicated than the C++ one, because complex 
 * types like vectors have to be manually managed and this header contains functions to 
 * convert C strings and generic sequences into respectively C++ strings and vectors.
 */
namespace CStringHelper
{
 /**
  * @brief Convert a rosidl_runtime_c__String into std::string
  *
  * This function converts a rosidl_runtime_c__String into a C++ string.
  *
  * @param str The original C-style string
  * @return    A C++ string
  */
  std::string convert_to_std_string(void * str);
 /**
  * @brief Convert a rosidl_runtime_c__String into a vector of std::string
  *
  * This function converts a rosidl_runtime_c__String fixed size sequence 
  * into a C++ vector of strings.
  *
  * @param str_array Pointer to the first original C-style string
  * @param size      Number of elements in the array
  * @return          A C++ vector of strings
  */
  std::vector<std::string> convert_to_std_vector_string(void * str_array, size_t size);
 /**
  * @brief Convert a rosidl_runtime_c__String__Sequence into a vector of std::string
  *
  * This function converts a rosidl_runtime_c__String__Sequence variable size sequence 
  * into a C++ vector of strings.
  *
  * @param str_seq Pointer to the first original C-style string
  * @return        A C++ vector of strings
  */
  std::vector<std::string> convert_sequence_to_std_vector_string(void * str_seq);
  
 /**
  * @brief Convert a rosidl_runtime_c__U16String into std::u16string
  *
  * This function converts a rosidl_runtime_c__U16String into a C++ u16string.
  *
  * @param str The original C-style u16string
  * @return    A C++ u16string
  */
  std::u16string convert_to_std_u16string(void * str);
 /**
  * @brief Convert a rosidl_runtime_c__U16String into a vector of std::u16string
  *
  * This function converts a rosidl_runtime_c__U16String fixed size sequence 
  * into a C++ vector of u16string.
  *
  * @param str_array Pointer to the first original C-style u16string
  * @param size      Number of elements in the array
  * @return          A C++ vector of u16strings
  */
  std::vector<std::u16string> convert_to_std_vector_u16string(void * str_array, size_t size);
 /**
  * @brief Convert a rosidl_runtime_c__U16String__Sequence into a vector of std::u16string
  *
  * This function converts a rosidl_runtime_c__U16String__Sequence variable size sequence 
  * into a C++ vector of u16string.
  *
  * @param str_seq Pointer to the first original C-style u16string
  * @return        A C++ vector of u16string
  */
  std::vector<std::u16string> convert_sequence_to_std_vector_u16string(void * str_seq);
  
 /**
  * @brief Assing to a rosidl_runtime_c__String the value contained in a std::string
  *
  * This function stores the data contained in a C++ string in a rosidl_runtime_c__String 
  * pointed by the field parameter.
  *
  * @param str   C++ style string containing data
  * @param field Pointer containing the destination of the string
  */
  void assign_string(std::string str, void * field);
 /**
  * @brief Assing to a rosidl_runtime_c__String the value contained in a vector of std::string
  *
  * This function stores the data contained in a C++ vector of strings in a rosidl_runtime_c__String 
  * fixed size sequence pointed by the str_array parameter.
  *
  * @param cpp_string_vector C++ style vector of string containing data
  * @param str_array         Pointer containing the destination of the string sequence
  * @param size              Number of elements in the array
  */
  void assign_vector_string(std::vector<std::string> cpp_string_vector, void * str_array, size_t size);
 /**
  * @brief Assing to a rosidl_runtime_c__String__Sequence the value contained in a vector of std::string
  *
  * This function stores the data contained in a C++ vector of strings in a rosidl_runtime_c__String__Sequence 
  * variable size sequence pointed by the str_array parameter.
  *
  * @param cpp_string_vector C++ style vector of string containing data
  * @param str_seq           Pointer containing the destination of the string sequence
  */
  void assign_vector_string_to_sequence(std::vector<std::string> cpp_string_vector, void * str_seq);
  
 /**
  * @brief Assing to a rosidl_runtime_c__U16String the value contained in a std::u16string
  *
  * This function stores the data contained in a C++ u16string in a rosidl_runtime_c__U16String 
  * pointed by the field parameter.
  *
  * @param str   C++ style u16string containing data
  * @param field Pointer containing the destination of the u16string
  */
  void assign_u16string(std::u16string str, void * field);
 /**
  * @brief Assing to a rosidl_runtime_c__U16String the value contained in a vector of std::u16string
  *
  * This function stores the data contained in a C++ vector of u16strings in a rosidl_runtime_c__U16String 
  * fixed size sequence pointed by the str_array parameter.
  *
  * @param cpp_string_vector C++ style vector of u16strings containing data
  * @param str_array         Pointer containing the destination of the u16string sequence
  * @param size              Number of elements in the array
  */
  void assign_vector_u16string(std::vector<std::u16string> cpp_string_vector, void * str_array, size_t size);
 /**
  * @brief Assing to a rosidl_runtime_c__U16String__Sequence the value contained in a vector of std::u16string
  *
  * This function stores the data contained in a C++ vector of u16strings in a rosidl_runtime_c__U16String__Sequence 
  * variable size sequence pointed by the str_array parameter.
  *
  * @param cpp_string_vector C++ style vector of u16strings containing data
  * @param str_seq           Pointer containing the destination of the u16string sequence
  */
  void assign_vector_u16string_to_sequence(std::vector<std::u16string> cpp_string_vector, void * str_seq);
}


#endif
