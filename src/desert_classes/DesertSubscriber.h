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
 * @file DesertSubscriber.h
 * @brief Implementation of the Subscriber structure for DESERT
 * 
 * The DesertSubscriber class is used to create instances of the various subscribers 
 * registered by ROS. Each of them contains the informations needed to decode the 
 * data structure of the messages in the topic through specific public functions.
 *
 * @author Prof. Davide Costa
 *
 */

#ifndef DESERT_SUBSCRIBER_H_
#define DESERT_SUBSCRIBER_H_

/** @cond */

#include "rosidl_typesupport_introspection_cpp/identifier.hpp"
#include "rosidl_typesupport_introspection_c/identifier.h"
#include "rosidl_typesupport_introspection_cpp/message_introspection.hpp"
#include "rosidl_typesupport_introspection_c/message_introspection.h"
#include "rosidl_typesupport_introspection_cpp/service_introspection.hpp"
#include "rosidl_typesupport_introspection_c/service_introspection.h"
#include "rosidl_typesupport_introspection_cpp/field_types.hpp"
#include "rosidl_typesupport_introspection_c/field_types.h"

#include "rosidl_typesupport_cpp/identifier.hpp"
#include "rosidl_typesupport_c/type_support_map.h"
#include "rosidl_typesupport_c/identifier.h"

#include "rosidl_runtime_c/message_type_support_struct.h"

#include <vector>
#include <string>

/** @endcond */

#include "CBorStream.h"
#include "MessageSerialization.h"

class DesertSubscriber
{
  public:
   /**
    * @brief Create a subscriber
    *
    * @param topic_name    Name of the topic used for the registration
    * @param type_supports Pointer to the message data structure coming from the ROS upper layers
    */
    DesertSubscriber(std::string topic_name, const rosidl_message_type_support_t * type_supports);
    
   /**
    * @brief Check if there is available data for the registered topic
    *
    * The has_data function calls the interpret_packets method in RxStream and then verifies 
    * if in the map of subscriber packets there is a correspondence with the topic name of the
    * current instance.
    *
    * @return True if data is present otherwise false
    */
    bool has_data();
   /**
    * @brief Read a publication from the publisher
    *
    * The read_data function interprets a transmission with the topic name present in the current 
    * instance deserializing the message using the method from the MessageSerialization namespace. 
    * A discrimination is made between C members and C++ members based on the type support.
    *
    * @param msg        Pointer to the memory location used to store the message
    */
    void read_data(void * msg);
  
  private:
    cbor::RxStream _data_stream;
    std::string _name;
    
    int _c_cpp_identifier;
    const void * _members;
    
    const void * get_members(const rosidl_message_type_support_t * type_support);
    const rosidl_message_type_support_t * get_type_support(const rosidl_message_type_support_t * type_supports);

};

#endif
