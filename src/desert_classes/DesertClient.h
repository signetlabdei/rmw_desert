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
 * @file DesertClient.h
 * @brief Implementation of the Client structure for DESERT
 * 
 * The DesertClient class is used to create instances of the various clients 
 * registered by ROS. Each of them contains the informations needed to decode 
 * the data structure of the messages in the service and allows to send and 
 * receive data through specific public functions.
 *
 * @author Prof. Davide Costa
 *
 */

#ifndef DESERT_CLIENT_H_
#define DESERT_CLIENT_H_

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

#include "rosidl_runtime_c/service_type_support_struct.h"

#include "rmw/types.h"

#include <vector>
#include <string>
#include <regex>

/** @endcond */

#include "CBorStream.h"
#include "MessageSerialization.h"

class DesertClient
{
  public:
   /**
    * @brief Create a client
    *
    * @param service_name  Name of the service to send requests and receive responses
    * @param type_supports Pointer to the message data structure coming from the ROS upper layers
    * @param gid           Global identifier of the entity
    */
    DesertClient(std::string service_name, const rosidl_service_type_support_t * type_supports, rmw_gid_t gid);
    
   /**
    * @brief Check if there is available data for the current client instance
    *
    * The has_data function calls the interpret_packets method in RxStream and then verifies 
    * if in the map of client packets there is a correspondence with the service name and the
    * sequence identifier of the current instance.
    *
    * @return True if data is present otherwise false
    */
    bool has_data();
   /**
    * @brief Send a request to the service
    *
    * The send_request function starts a transmission with the current sequence identifier 
    * and then serializes the message using the method from the MessageSerialization namespace. 
    * A discrimination is made between C members and C++ members based on the type support.
    *
    * @param req         Pointer to the request to send
    * @param sequence_id Pointer to the random service sequence identifier
    */
    void send_request(const void * req, int64_t * sequence_id);
   /**
    * @brief Read a response from the service
    *
    * The read_response function interprets a transmission with the current sequence identifier 
    * deserializing the message using the method from the MessageSerialization namespace. 
    * A discrimination is made between C members and C++ members based on the type support.
    *
    * @param res        Pointer to the memory location used to store the reading
    * @param req_header Pointer to the request header used to store the service sequence identifier
    */
    void read_response(void * res, rmw_service_info_t * req_header);
    
   /**
    * @brief Retreive the gid of the current entity
    *
    * This function returns the global identifier of the current entity in the rmw format.
    *
    * @return Global identifier of the entity
    */
    rmw_gid_t get_gid();
   /**
    * @brief Retreive the service name of the current entity
    *
    * This function returns a string containing the service name of the current entity.
    *
    * @return Name of the service
    */
    std::string get_service_name();
   /**
    * @brief Retreive the request type of the current entity
    *
    * This function returns a string containing the service request type name of the current entity.
    *
    * @return Type of the service request
    */
    std::string get_request_type_name();
   /**
    * @brief Retreive the response type of the current entity
    *
    * This function returns a string containing the service response type name of the current entity.
    *
    * @return Type of the service response
    */
    std::string get_response_type_name();
    
    
  private:
    uint8_t _id;
    rmw_gid_t _gid;
    std::string _name;
    cbor::TxStream _request_data_stream;
    cbor::RxStream _response_data_stream;
    
    int64_t _sequence_id;
    
    int _c_cpp_identifier;
    const void * _service;
    
    const void * get_service(const rosidl_service_type_support_t * service_type_support);
    const rosidl_service_type_support_t * get_service_type_support(const rosidl_service_type_support_t * type_supports);

};

#endif
