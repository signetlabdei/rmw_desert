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
 * @file DesertPublisher.h
 * @brief Implementation of the Publisher structure for DESERT
 * 
 * The DesertPublisher class is used to create instances of the various publishers 
 * registered by ROS. Each of them contains the informations needed to encode the 
 * data structure of the messages in the topic and send them to the stream through 
 * specific public functions.
 *
 * @author Prof. Davide Costa
 *
 */

#ifndef DESERT_PUBLISHER_H_
#define DESERT_PUBLISHER_H_

/** @cond */

#include "rosidl_typesupport_introspection_cpp/identifier.hpp"
#include "rosidl_typesupport_introspection_c/identifier.h"
#include "rosidl_typesupport_introspection_cpp/message_introspection.hpp"
#include "rosidl_typesupport_introspection_c/message_introspection.h"
#include "rosidl_typesupport_introspection_cpp/service_introspection.hpp"
#include "rosidl_typesupport_introspection_c/service_introspection.h"
#include "rosidl_typesupport_introspection_cpp/field_types.hpp"
#include "rosidl_typesupport_introspection_c/field_types.h"

#include "rosidl_runtime_c/message_type_support_struct.h"

#include "rmw/types.h"

#include <vector>
#include <string>
#include <regex>

/** @endcond */

#include "CBorStream.h"
#include "MessageSerialization.h"

class DesertPublisher
{
  public:
   /**
    * @brief Create a publisher
    *
    * @param topic_name    Name of the topic used to push the messages
    * @param type_supports Pointer to the message data structure coming from the ROS upper layers
    * @param gid           Global identifier of the entity
    */
    DesertPublisher(std::string topic_name, const rosidl_message_type_support_t * type_supports, rmw_gid_t gid);
    
   /**
    * @brief Send a publication on the topic
    *
    * The push function starts a transmission with the topic name in the current instance 
    * and then serializes the message using the method from the MessageSerialization namespace. 
    * A discrimination is made between C members and C++ members based on the type support.
    *
    * @param msg         Pointer to the message to send
    */
    void push(const void * msg);
    
   /**
    * @brief Retreive the gid of the current entity
    *
    * This function returns the global identifier of the current entity in the rmw format.
    *
    * @return Global identifier of the entity
    */
    rmw_gid_t get_gid();
   /**
    * @brief Retreive the topic name of the current entity
    *
    * This function returns a string containing the topic name of the current entity.
    *
    * @return Name of the topic
    */
    std::string get_topic_name();
   /**
    * @brief Retreive the message type of the current entity
    *
    * This function returns a string containing the message type name of the current entity.
    *
    * @return Type of the message
    */
    std::string get_type_name();
    
    
  private:
    uint8_t _id;
    rmw_gid_t _gid;
    std::string _name;
    cbor::TxStream _data_stream;
    
    int _c_cpp_identifier;
    const void * _members;
    
    const void * get_members(const rosidl_message_type_support_t * type_support);
    const rosidl_message_type_support_t * get_type_support(const rosidl_message_type_support_t * type_supports);

};

#endif
