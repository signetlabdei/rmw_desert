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
 * @file MessageSerialization.h
 * @brief Namespace containing serialization functions
 * 
 * The message data structure coming from upper layers is interpreted using type 
 * support informations passed by ROS2 during the creation of publishers, subscribers, 
 * clients and services. Those functions are used to compute the exact position that 
 * every data type must assume in memory an then calls TxStream or RxStream to receive 
 * or write them in the assigned location.
 *
 * @author Prof. Davide Costa
 *
 */

#ifndef MESSAGE_SERIALIZATION_H_
#define MESSAGE_SERIALIZATION_H_

#include "CBorStream.h"
#include "CStringHelper.h"
#include "macros.h"

/** @cond */

#include <vector>
#include <string>

/** @endcond */

#define INTROSPECTION_C_MEMBER rosidl_typesupport_introspection_c__MessageMember
#define INTROSPECTION_CPP_MEMBER rosidl_typesupport_introspection_cpp::MessageMember

#define INTROSPECTION_C_MEMBERS rosidl_typesupport_introspection_c__MessageMembers
#define INTROSPECTION_CPP_MEMBERS rosidl_typesupport_introspection_cpp::MessageMembers

#define INTROSPECTION_C_SERVICE_MEMBERS rosidl_typesupport_introspection_c__ServiceMembers
#define INTROSPECTION_CPP_SERVICE_MEMBERS rosidl_typesupport_introspection_cpp::ServiceMembers

/**
 * @namespace MessageSerialization
 * @brief Namespace containing serialization functions
 * 
 * The message data structure coming from upper layers is interpreted using type 
 * support informations passed by ROS2 during the creation of publishers, subscribers, 
 * clients and services. Those functions are used to compute the exact position that 
 * every data type must assume in memory an then calls TxStream or RxStream to receive 
 * or write them in the assigned location.
 */
namespace MessageSerialization
{

 /**
  * @brief Serialize a C++ field
  *
  * The type support introspection information is used to know if a specific 
  * data type is a single item, a sequence or a vector. Based on this conclusion 
  * a specific interpretation is passed to the stream.
  *
  * @param member  Pointer to the member containing type support informations
  * @param field   Pointer to the origin memory address of the elementary data
  * @param stream  The stream used to send data
  */
  template<typename T>
  void serialize_field(const INTROSPECTION_CPP_MEMBER * member, void * field, cbor::TxStream & stream)
  {
    if (!member->is_array_)
    {
      stream << * static_cast<T *>(field);
    }
    else if (member->array_size_ && !member->is_upper_bound_)
    {
      stream.serialize_sequence(static_cast<T *>(field), member->array_size_);
    }
    else
    {
      std::vector<T> & data = *reinterpret_cast<std::vector<T> *>(field);
      stream << data;
    }
  }

 /**
  * @brief Serialize a C field
  *
  * The type support introspection information is used to know if a specific 
  * data type is a single item, a sequence or a variable length sequence. 
  * Based on this conclusion a specific interpretation is passed to the stream.
  *
  * @param member  Pointer to the member containing type support informations
  * @param field   Pointer to the origin memory address of the elementary data
  * @param stream  The stream used to send data
  */
  template<typename T>
  void serialize_field(const INTROSPECTION_C_MEMBER * member, void * field, cbor::TxStream & stream)
  {
    // String specific implementation
    if constexpr(std::is_same_v<T, std::string>)
    {
      if (!member->is_array_) 
      {
        stream << CStringHelper::convert_to_std_string(field);
      }
      else if (member->array_size_ && !member->is_upper_bound_)
      {
        stream << CStringHelper::convert_to_std_vector_string(field, member->array_size_);
      }
      else
      {
        printf("WARNING: non-fixed size sequences are currently sperimental\n");
        stream << CStringHelper::convert_sequence_to_std_vector_string(field);
      }
    }
    // U16string specific implementation
    else if constexpr(std::is_same_v<T, std::u16string>)
    {
      if (!member->is_array_) 
      {
        stream << CStringHelper::convert_to_std_u16string(field);
      }
      else if (member->array_size_ && !member->is_upper_bound_)
      {
        stream << CStringHelper::convert_to_std_vector_u16string(field, member->array_size_);
      }
      else
      {
        printf("WARNING: non-fixed size sequences are currently sperimental\n");
        stream << CStringHelper::convert_sequence_to_std_vector_u16string(field);
      }
    }
    // Generic implementation
    else
    {
      if (!member->is_array_) 
      {
        stream << * static_cast<T *>(field);
      }
      else if (member->array_size_ && !member->is_upper_bound_)
      {
        stream.serialize_sequence(static_cast<T *>(field), member->array_size_);
      }
      else
      {
        printf("WARNING: non-fixed size sequences are currently sperimental\n");
        auto & data = *reinterpret_cast<typename GenericCSequence<T>::type *>(field);
        
        // Serialize length
        stream << (uint32_t)data.size;
              
        stream.serialize_sequence(reinterpret_cast<T *>(data.data), data.size);
      }
    }
  }
  
 /**
  * @brief Serialize a ROS message, request or response
  *
  * Every time ROS has data to send in the channel a memory location is 
  * passed with the corresponding message member type, and this function 
  * separates all the fields into elementary C or C++ types. Then the 
  * serialize_field function is called to encode the specific data.
  *
  * @param msg            Pointer to the first byte of the message in memory
  * @param casted_members Pointer to the member containing type support informations
  * @param stream         The stream used to send data
  */
  template<typename MembersType>
  void serialize(const void * msg, const MembersType * casted_members, cbor::TxStream & stream)
  {
    for (uint32_t i = 0; i < casted_members->member_count_; ++i) {
      const auto member = casted_members->members_ + i;
      void * field = const_cast<char *>(static_cast<const char *>(msg)) + member->offset_;
      switch (member->type_id_) {
        case ::rosidl_typesupport_introspection_cpp::ROS_TYPE_MESSAGE:
          {
            auto sub_members = static_cast<const MembersType *>(member->members_->data);
            if (!member->is_array_) {
              serialize(field, sub_members, stream);
            }
            else if (member->array_size_ && !member->is_upper_bound_)
            {
              for (size_t index = 0; index < member->array_size_; ++index) {
                serialize(member->get_function(field, index), sub_members, stream);
              }
            }
            else
            {
              size_t array_size = member->size_function(field);
              
              if (member->is_upper_bound_ && array_size > member->array_size_)
              {
                throw std::runtime_error("Sequence overcomes the maximum length");
              }
              
              // Serialize length
              stream << (uint32_t)array_size;
              
              for (size_t index = 0; index < array_size; ++index) {
                serialize(member->get_function(field, index), sub_members, stream);
              }
            }
          }
          break;
        case ::rosidl_typesupport_introspection_cpp::ROS_TYPE_BOOLEAN:
          if (!member->is_array_)
          {
            // Don't cast to bool here because if the bool is uninitialized the random value can't be deserialized
            stream << (*static_cast<uint8_t *>(field) ? true : false);
          }
          else
          {
            serialize_field<bool>(member, field, stream);
          }
          break;
        case ::rosidl_typesupport_introspection_cpp::ROS_TYPE_OCTET:
          //throw std::runtime_error("OCTET type unsupported");
          break;
        case ::rosidl_typesupport_introspection_cpp::ROS_TYPE_UINT8:
          serialize_field<uint8_t>(member, field, stream);
          break;
        case ::rosidl_typesupport_introspection_cpp::ROS_TYPE_CHAR:
          serialize_field<char>(member, field, stream);
          break;
        case ::rosidl_typesupport_introspection_cpp::ROS_TYPE_INT8:
          serialize_field<int8_t>(member, field, stream);
          break;
        case ::rosidl_typesupport_introspection_cpp::ROS_TYPE_FLOAT:
          serialize_field<float>(member, field, stream);
          break;
        case ::rosidl_typesupport_introspection_cpp::ROS_TYPE_DOUBLE:
          serialize_field<double>(member, field, stream);
          break;
        case ::rosidl_typesupport_introspection_cpp::ROS_TYPE_INT16:
          serialize_field<int16_t>(member, field, stream);
          break;
        case ::rosidl_typesupport_introspection_cpp::ROS_TYPE_UINT16:
          serialize_field<uint16_t>(member, field, stream);
          break;
        case ::rosidl_typesupport_introspection_cpp::ROS_TYPE_INT32:
          serialize_field<int32_t>(member, field, stream);
          break;
        case ::rosidl_typesupport_introspection_cpp::ROS_TYPE_UINT32:
          serialize_field<uint32_t>(member, field, stream);
          break;
        case ::rosidl_typesupport_introspection_cpp::ROS_TYPE_INT64:
          serialize_field<int64_t>(member, field, stream);
          break;
        case ::rosidl_typesupport_introspection_cpp::ROS_TYPE_UINT64:
          serialize_field<uint64_t>(member, field, stream);
          break;
        case ::rosidl_typesupport_introspection_cpp::ROS_TYPE_STRING:
          serialize_field<std::string>(member, field, stream);
          break;
        case ::rosidl_typesupport_introspection_cpp::ROS_TYPE_WSTRING:
          serialize_field<std::u16string>(member, field, stream);
          break;
        default:
          throw std::runtime_error("unknown type");
      }
    }
  }

 /**
  * @brief Deserialize a C++ field
  *
  * The type support introspection information is used to know if a specific 
  * data type is a single item, a sequence or a vector. Based on this conclusion 
  * a specific interpretation is passed to the stream.
  *
  * @param member  Pointer to the member containing type support informations
  * @param field   Pointer to the destination memory address of the elementary data
  * @param stream  The stream used to receive data
  */
  template<typename T>
  void deserialize_field(const INTROSPECTION_CPP_MEMBER * member, void * field, cbor::RxStream & stream)
  {
    if (!member->is_array_) {
      stream >> *static_cast<T *>(field);
    }
    else if (member->array_size_ && !member->is_upper_bound_)
    {
      stream.deserialize_sequence(static_cast<T *>(field), member->array_size_);
    }
    else
    {
      auto & vector = *reinterpret_cast<std::vector<T> *>(field);
      new(&vector) std::vector<T>;
      stream >> vector;
    }
  }

 /**
  * @brief Deserialize a C field
  *
  * The type support introspection information is used to know if a specific 
  * data type is a single item, a sequence or a variable length sequence. 
  * Based on this conclusion a specific interpretation is passed to the stream.
  *
  * @param member  Pointer to the member containing type support informations
  * @param field   Pointer to the destination memory address of the elementary data
  * @param stream  The stream used to receive data
  */
  template<typename T>
  void deserialize_field(const INTROSPECTION_C_MEMBER * member, void * field, cbor::RxStream & stream)
  {
    // String specific implementation
    if constexpr(std::is_same_v<T, std::string>)
    {
      if (!member->is_array_) 
      {
        std::string str;
        stream >> str;
        CStringHelper::assign_string(str, field);
      }
      else if (member->array_size_ && !member->is_upper_bound_)
      {
        std::vector<std::string> cpp_string_vector;
        stream >> cpp_string_vector;
        
        CStringHelper::assign_vector_string(cpp_string_vector, field, member->array_size_);
      }
      else
      {
        printf("WARNING: non-fixed size sequences are currently sperimental\n");
        std::vector<std::string> cpp_string_vector;
        stream >> cpp_string_vector;
        
        CStringHelper::assign_vector_string_to_sequence(cpp_string_vector, field);
      }
    }
    // U16string specific implementation
    else if constexpr(std::is_same_v<T, std::u16string>)
    {
      if (!member->is_array_) 
      {
        std::u16string str;
        stream >> str;
        CStringHelper::assign_u16string(str, field);
      }
      else if (member->array_size_ && !member->is_upper_bound_)
      {
        std::vector<std::u16string> cpp_string_vector;
        stream >> cpp_string_vector;
        
        CStringHelper::assign_vector_u16string(cpp_string_vector, field, member->array_size_);
      }
      else
      {
        printf("WARNING: non-fixed size sequences are currently sperimental\n");
        std::vector<std::u16string> cpp_string_vector;
        stream >> cpp_string_vector;
        
        CStringHelper::assign_vector_u16string_to_sequence(cpp_string_vector, field);
      }
    }
    // Generic implementation
    else
    {
      if (!member->is_array_) 
      {
        stream >> * static_cast<T *>(field);
      }
      else if (member->array_size_ && !member->is_upper_bound_)
      {
        stream.deserialize_sequence(static_cast<T *>(field), member->array_size_);
      }
      else
      {
        printf("WARNING: non-fixed size sequences are currently sperimental\n");
        auto & data = *reinterpret_cast<typename GenericCSequence<T>::type *>(field);
        uint32_t size = 0;
        stream >> size;
        size_t dsize = static_cast<size_t>(size);
        
        if (!GenericCSequence<T>::init(&data, dsize))
        {
          throw std::runtime_error("unable to initialize GenericCSequence");
        }
        
        stream.deserialize_sequence(reinterpret_cast<T *>(data.data), dsize);
      }
    }
  }

 /**
  * @brief Deserialize a ROS message, request or response
  *
  * Every time DESERT receives data from the channel a memory location is 
  * used to store the corresponding member type, and this function merges 
  * all the elementary C or C++ types into the whole message. To perform 
  * this operation the deserialize_field function is called to decode every 
  * specific data.
  *
  * @param msg            Pointer to the first byte of the message in memory
  * @param casted_members Pointer to the member containing type support informations
  * @param stream         The stream used to receive data
  */
  template<typename MembersType>
  void deserialize(void * msg, const MembersType * casted_members, cbor::RxStream & stream)
  {
    for (uint32_t i = 0; i < casted_members->member_count_; ++i) {
      const auto member = casted_members->members_ + i;
      void * field = static_cast<char *>(msg) + member->offset_;
      switch (member->type_id_) {
        case ::rosidl_typesupport_introspection_cpp::ROS_TYPE_MESSAGE:
          {
            auto sub_members = static_cast<const MembersType *>(member->members_->data);
            if (!member->is_array_) {
              deserialize(field, sub_members, stream);
            }
            else if (member->array_size_ && !member->is_upper_bound_)
            {
              for (size_t index = 0; index < member->array_size_; ++index) {
                deserialize(member->get_function(field, index), sub_members, stream);
              }
            }
            else
            {
              // Deserialize length
              uint32_t array_size = 0;
              stream >> array_size;
              
              auto vector = reinterpret_cast<std::vector<unsigned char> *>(field);
              new(vector) std::vector<unsigned char>;
              member->resize_function(field, array_size);
              
              for (size_t index = 0; index < array_size; ++index) {
                deserialize(member->get_function(field, index), sub_members, stream);
              }
            }
          }
          break;
        case ::rosidl_typesupport_introspection_cpp::ROS_TYPE_BOOLEAN:
          deserialize_field<bool>(member, field, stream);
          break;
        case ::rosidl_typesupport_introspection_cpp::ROS_TYPE_OCTET:
          //throw std::runtime_error("OCTET type unsupported");
          break;
        case ::rosidl_typesupport_introspection_cpp::ROS_TYPE_UINT8:
          deserialize_field<uint8_t>(member, field, stream);
          break;
        case ::rosidl_typesupport_introspection_cpp::ROS_TYPE_CHAR:
          deserialize_field<char>(member, field, stream);
          break;
        case ::rosidl_typesupport_introspection_cpp::ROS_TYPE_INT8:
          deserialize_field<int8_t>(member, field, stream);
          break;
        case ::rosidl_typesupport_introspection_cpp::ROS_TYPE_FLOAT:
          deserialize_field<float>(member, field, stream);
          break;
        case ::rosidl_typesupport_introspection_cpp::ROS_TYPE_DOUBLE:
          deserialize_field<double>(member, field, stream);
          break;
        case ::rosidl_typesupport_introspection_cpp::ROS_TYPE_INT16:
          deserialize_field<int16_t>(member, field, stream);
          break;
        case ::rosidl_typesupport_introspection_cpp::ROS_TYPE_UINT16:
          deserialize_field<uint16_t>(member, field, stream);
          break;
        case ::rosidl_typesupport_introspection_cpp::ROS_TYPE_INT32:
          deserialize_field<int32_t>(member, field, stream);
          break;
        case ::rosidl_typesupport_introspection_cpp::ROS_TYPE_UINT32:
          deserialize_field<uint32_t>(member, field, stream);
          break;
        case ::rosidl_typesupport_introspection_cpp::ROS_TYPE_INT64:
          deserialize_field<int64_t>(member, field, stream);
          break;
        case ::rosidl_typesupport_introspection_cpp::ROS_TYPE_UINT64:
          deserialize_field<uint64_t>(member, field, stream);
          break;
        case ::rosidl_typesupport_introspection_cpp::ROS_TYPE_STRING:
          deserialize_field<std::string>(member, field, stream);
          break;
        case ::rosidl_typesupport_introspection_cpp::ROS_TYPE_WSTRING:
          deserialize_field<std::u16string>(member, field, stream);
          break;
        default:
          throw std::runtime_error("unknown type");
      }
    }
  }

}


#endif
