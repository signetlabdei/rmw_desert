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
 * @file ProtobufHelper.h
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

#ifndef PROTOBUF_HELPER_H_
#define PROTOBUF_HELPER_H_

/** @cond */

#include <vector>
#include <string>

/** @endcond */

/**
 * @namespace ProtobufHelper
 * @brief Namespace containing serialization functions
 * 
 * The message data structure coming from upper layers is interpreted using type 
 * support informations passed by ROS2 during the creation of publishers, subscribers, 
 * clients and services. Those functions are used to compute the exact position that 
 * every data type must assume in memory an then calls TxStream or RxStream to receive 
 * or write them in the assigned location.
 */
namespace ProtobufHelper
{

  inline int submessage_counter;
  inline std::string custom_types_proto;
  
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
  std::string rosidl_to_proto(const MembersType * casted_members, bool is_service, bool first_run)
  {
    std::string proto("");
    if (first_run)
    {
      custom_types_proto = "";
      proto = ""
        "syntax = \"proto2\";\n\n\n"

        "message DynamicMessage {\n"
        "  option (dccl.msg) = { codec_version: 4\n"
        "                        id: " + std::to_string(submessage_counter) + "\n"
        "                        max_bytes: 512000 };\n\n"
        "  required uint32 stream_type = 1 [(dccl.field) = { min: 0 max: 3 }];\n";
      if (is_service)
      {
        proto.append("  required uint64 sequence_id = 2 [(dccl.field) = { min: 1 max: 1000 }];\n");
      }
    }
    
    for (uint32_t i = 0; i < casted_members->member_count_; ++i) {
      const auto member = casted_members->members_ + i;
      int proto_variable_number = i + 1 + 1 * first_run + 1 * is_service;
      
      switch (member->type_id_) {
        case ::rosidl_typesupport_introspection_cpp::ROS_TYPE_MESSAGE:
          {
            auto sub_members = static_cast<const MembersType *>(member->members_->data);
            std::string custom_type = rosidl_to_proto(sub_members, false, false);
            std::string type_name = static_cast<const MembersType *>(member->members_->data)->message_name_;
            
            if (!member->is_array_)
            {
              if (!custom_types_proto.contains("message " + type_name + " {"))
              {
                submessage_counter++;
                custom_types_proto.append("\nmessage " + type_name + " {\n" +
                                          "  option (dccl.msg) = { codec_version: 4\n" +
                                          "                        id: " + std::to_string(submessage_counter) + "\n" +
                                          "                        max_bytes: 512 };\n\n" +
                                           custom_type +
                                           "}\n");
              }
              proto.append("  optional " + type_name + " " + member->name_ + " = " + std::to_string(proto_variable_number) + ";\n");
            }
            else if (member->array_size_ && !member->is_upper_bound_)
            {
              if (!custom_types_proto.contains("message " + type_name + " {"))
              {
                submessage_counter++;
                custom_types_proto.append("\nmessage " + type_name + " {\n" +
                                          "  option (dccl.msg) = { codec_version: 4\n" +
                                          "                        id: " + std::to_string(submessage_counter) + "\n" +
                                          "                        max_bytes: 512 };\n\n" +
                                           custom_type +
                                           "}\n");
              }
              proto.append("  repeated " + type_name + " " + member->name_ + " = " + std::to_string(proto_variable_number) + " [(dccl.field) = { max_repeat: 32 }];\n");
            }
            else
            {
              // Variable length sequence of custom types
              
              if (!custom_types_proto.contains("message " + type_name + " {"))
              {
                submessage_counter++;
                custom_types_proto.append("\nmessage " + type_name + " {\n" +
                                          "  option (dccl.msg) = { codec_version: 4\n" +
                                          "                        id: " + std::to_string(submessage_counter) + "\n" +
                                          "                        max_bytes: 512 };\n\n" +
                                           custom_type +
                                           "}\n");
              }
              proto.append("  repeated " + type_name + " " + member->name_ + " = " + std::to_string(proto_variable_number) + " [(dccl.field) = { max_repeat: 32 }];\n");
            }
          }
          break;
        case ::rosidl_typesupport_introspection_cpp::ROS_TYPE_BOOLEAN:
          if (!member->is_array_)
          {
            proto.append("  optional bool " + std::string(member->name_) + " = " + std::to_string(proto_variable_number) + ";\n");
          }
          else
          {
            proto.append("  repeated bool " + std::string(member->name_) + " = " + std::to_string(proto_variable_number) + " [(dccl.field) = { max_repeat: 32 }];\n");
          }
          break;
        case ::rosidl_typesupport_introspection_cpp::ROS_TYPE_OCTET:
          //throw std::runtime_error("OCTET type unsupported");
          break;
        case ::rosidl_typesupport_introspection_cpp::ROS_TYPE_UINT8:
          if (!member->is_array_)
          {
            proto.append("  optional uint32 " + std::string(member->name_) + " = " + std::to_string(proto_variable_number) + " [(dccl.field) = { min: 0 max: 255 }];\n");
          }
          else
          {
            proto.append("  repeated uint32 " + std::string(member->name_) + " = " + std::to_string(proto_variable_number) + " [(dccl.field) = { min: 0 max: 255 max_repeat: 32 }];\n");
          }
          break;
        case ::rosidl_typesupport_introspection_cpp::ROS_TYPE_CHAR:
          if (!member->is_array_)
          {
            proto.append("  optional uint32 " + std::string(member->name_) + " = " + std::to_string(proto_variable_number) + " [(dccl.field) = { min: 0 max: 255 }];\n");
          }
          else
          {
            proto.append("  repeated uint32 " + std::string(member->name_) + " = " + std::to_string(proto_variable_number) + " [(dccl.field) = { min: 0 max: 255 max_repeat: 32 }];\n");
          }
          break;
        case ::rosidl_typesupport_introspection_cpp::ROS_TYPE_INT8:
          if (!member->is_array_)
          {
            proto.append("  optional int32 " + std::string(member->name_) + " = " + std::to_string(proto_variable_number) + " [(dccl.field) = { min: -128 max: 127 }];\n");
          }
          else
          {
            proto.append("  repeated int32 " + std::string(member->name_) + " = " + std::to_string(proto_variable_number) + " [(dccl.field) = { min: -128 max: 127 max_repeat: 32 }];\n");
          }
          break;
        case ::rosidl_typesupport_introspection_cpp::ROS_TYPE_FLOAT:
          if (!member->is_array_)
          {
            proto.append("  optional float " + std::string(member->name_) + " = " + std::to_string(proto_variable_number) + " [(dccl.field) = { min: -10000000 max: 10000000 precision: 2 }];\n");
          }
          else
          {
            proto.append("  repeated float " + std::string(member->name_) + " = " + std::to_string(proto_variable_number) + " [(dccl.field) = { min: -10000000 max: 10000000 precision: 2 max_repeat: 32 }];\n");
          }
          break;
        case ::rosidl_typesupport_introspection_cpp::ROS_TYPE_DOUBLE:
          if (!member->is_array_)
          {
            proto.append("  optional double " + std::string(member->name_) + " = " + std::to_string(proto_variable_number) + " [(dccl.field) = { min: -10000000 max: 10000000 precision: 4 }];\n");
          }
          else
          {
            proto.append("  repeated double " + std::string(member->name_) + " = " + std::to_string(proto_variable_number) + " [(dccl.field) = { min: -10000000 max: 10000000 precision: 4 max_repeat: 32 }];\n");
          }
          break;
        case ::rosidl_typesupport_introspection_cpp::ROS_TYPE_INT16:
          if (!member->is_array_)
          {
            proto.append("  optional int32 " + std::string(member->name_) + " = " + std::to_string(proto_variable_number) + " [(dccl.field) = { min: -32768 max: 32767 }];\n");
          }
          else
          {
            proto.append("  repeated int32 " + std::string(member->name_) + " = " + std::to_string(proto_variable_number) + " [(dccl.field) = { min: -32768 max: 32767 max_repeat: 32 }];\n");
          }
          break;
        case ::rosidl_typesupport_introspection_cpp::ROS_TYPE_UINT16:
          if (!member->is_array_)
          {
            proto.append("  optional uint32 " + std::string(member->name_) + " = " + std::to_string(proto_variable_number) + " [(dccl.field) = { min: 0 max: 65535 }];\n");
          }
          else
          {
            proto.append("  repeated uint32 " + std::string(member->name_) + " = " + std::to_string(proto_variable_number) + " [(dccl.field) = { min: 0 max: 65535 max_repeat: 32 }];\n");
          }
          break;
        case ::rosidl_typesupport_introspection_cpp::ROS_TYPE_INT32:
          if (!member->is_array_)
          {
            proto.append("  optional int32 " + std::string(member->name_) + " = " + std::to_string(proto_variable_number) + " [(dccl.field) = { min: -2147483648 max: 2147483647 }];\n");
          }
          else
          {
            proto.append("  repeated int32 " + std::string(member->name_) + " = " + std::to_string(proto_variable_number) + " [(dccl.field) = { min: -2147483648 max: 2147483647 max_repeat: 32 }];\n");
          }
          break;
        case ::rosidl_typesupport_introspection_cpp::ROS_TYPE_UINT32:
          if (!member->is_array_)
          {
            proto.append("  optional uint32 " + std::string(member->name_) + " = " + std::to_string(proto_variable_number) + " [(dccl.field) = { min: 0 max: 4294967295 }];\n");
          }
          else
          {
            proto.append("  repeated uint32 " + std::string(member->name_) + " = " + std::to_string(proto_variable_number) + " [(dccl.field) = { min: 0 max: 4294967295 max_repeat: 32 }];\n");
          }
          break;
        case ::rosidl_typesupport_introspection_cpp::ROS_TYPE_INT64:
          if (!member->is_array_)
          {
            proto.append("  optional int64 " + std::string(member->name_) + " = " + std::to_string(proto_variable_number) + " [(dccl.field) = { min: -2147483648 max: 2147483647 }];\n");
          }
          else
          {
            proto.append("  repeated int64 " + std::string(member->name_) + " = " + std::to_string(proto_variable_number) + " [(dccl.field) = { min: -2147483648 max: 2147483647 max_repeat: 32 }];\n");
          }
          break;
        case ::rosidl_typesupport_introspection_cpp::ROS_TYPE_UINT64:
          if (!member->is_array_)
          {
            proto.append("  optional uint64 " + std::string(member->name_) + " = " + std::to_string(proto_variable_number) + " [(dccl.field) = { min: 0 max: 18446744073709551615 }];\n");
          }
          else
          {
            proto.append("  repeated uint64 " + std::string(member->name_) + " = " + std::to_string(proto_variable_number) + " [(dccl.field) = { min: 0 max: 18446744073709551615 max_repeat: 32 }];\n");
          }
          break;
        case ::rosidl_typesupport_introspection_cpp::ROS_TYPE_STRING:
          if (!member->is_array_)
          {
            proto.append("  optional string " + std::string(member->name_) + " = " + std::to_string(proto_variable_number) + " [(dccl.field) = { max_length: 64 }];\n");
          }
          else
          {
            proto.append("  repeated string " + std::string(member->name_) + " = " + std::to_string(proto_variable_number) + " [(dccl.field) = { max_length: 64 max_repeat: 32 }];\n");
          }
          break;
        case ::rosidl_typesupport_introspection_cpp::ROS_TYPE_WSTRING:
          if (!member->is_array_)
          {
            proto.append("  optional string " + std::string(member->name_) + " = " + std::to_string(proto_variable_number) + " [(dccl.field) = { max_length: 64 }];\n");
          }
          else
          {
            proto.append("  repeated string " + std::string(member->name_) + " = " + std::to_string(proto_variable_number) + " [(dccl.field) = { max_length: 64 max_repeat: 32 }];\n");
          }
          break;
        //default:
          //throw std::runtime_error("unknown type");
      }
    }
    
    if (first_run)
    {
      proto.insert(20, custom_types_proto);
      proto.append("}");
    }
    
    return proto;
  }
  
  inline std::string rosidl_to_proto(const void * members, int sender_type, int c_cpp_identifier, int id)
  {
    if (id == 0) return "";
    
    submessage_counter = id;
    
    if (sender_type == PUBLISHER_TYPE)
    {
      if (c_cpp_identifier == 0)
      {
        auto casted_members = static_cast<const INTROSPECTION_C_MEMBERS *>(members);
        return rosidl_to_proto(casted_members, false, true);
      }
      else if (c_cpp_identifier == 1)
      {
        auto casted_members = static_cast<const INTROSPECTION_CPP_MEMBERS *>(members);
        return rosidl_to_proto(casted_members, false, true);
      }
    }
    if (sender_type == CLIENT_TYPE)
    {
      if (c_cpp_identifier == 0)
      {
        auto casted_service = static_cast<const INTROSPECTION_C_SERVICE_MEMBERS *>(members);
        return rosidl_to_proto(casted_service->request_members_, true, true);
      }
      else if (c_cpp_identifier == 1)
      {
        auto casted_service = static_cast<const INTROSPECTION_CPP_SERVICE_MEMBERS *>(members);
        return rosidl_to_proto(casted_service->request_members_, true, true);
      }
    }
    if (sender_type == SERVICE_TYPE)
    {
      if (c_cpp_identifier == 0)
      {
        auto casted_service = static_cast<const INTROSPECTION_C_SERVICE_MEMBERS *>(members);
        return rosidl_to_proto(casted_service->response_members_, true, true);
      }
      else if (c_cpp_identifier == 1)
      {
        auto casted_service = static_cast<const INTROSPECTION_CPP_SERVICE_MEMBERS *>(members);
        return rosidl_to_proto(casted_service->response_members_, true, true);
      }
    }
    return "";
  }


}


#endif
