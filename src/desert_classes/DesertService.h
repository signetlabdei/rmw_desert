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
 * @file DesertService.h
 * @brief Implementation of the Service structure for DESERT
 * 
 * The DesertService class is used to create instances of the various services 
 * registered by ROS. Each of them contains the informations needed to decode 
 * the data structure of the messages in the stream and allows to send and 
 * receive data through specific public functions.
 *
 * @author Prof. Davide Costa
 *
 */

#ifndef DESERT_SERVICE_H_
#define DESERT_SERVICE_H_

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

class DesertService
{
  public:
   /**
    * @brief Create a service
    *
    * @param service_name  Name of the service to receive requests and send responses
    * @param type_supports Pointer to the message data structure coming from the ROS upper layers
    * @param gid           Global identifier of the entity
    */
    DesertService(std::string service_name, const rosidl_service_type_support_t * type_supports, rmw_gid_t gid);
    
   /**
    * @brief Check if there is available data for the current service instance
    *
    * The has_data function calls the interpret_packets method in RxStream and then verifies 
    * if in the map of service packets there is a correspondence with the service name of the
    * current instance.
    *
    * @return True if data is present otherwise false
    */
    bool has_data();
   /**
    * @brief Read a request from a client
    *
    * The read_request function interprets a transmission with the service name in the current 
    * instance deserializing the message using the method from the MessageSerialization namespace. 
    * A discrimination is made between C members and C++ members based on the type support.
    *
    * @param req        Pointer to the memory location used to store the request
    * @param req_header Pointer to the request header used to store the service sequence identifier
    */
    void read_request(void * req, rmw_service_info_t * req_header);
   /**
    * @brief Send the response to a client
    *
    * The send_response function starts a transmission with the sequence identifier in req_header 
    * and then serializes the message using the method from the MessageSerialization namespace. 
    * A discrimination is made between C members and C++ members based on the type support.
    *
    * @param res         Pointer to the response to send
    * @param req_header  Pointer to the request header used to store the service sequence identifier
    */
    void send_response(void * res, rmw_request_id_t * req_header);
    
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
    cbor::RxStream _request_data_stream;
    cbor::TxStream _response_data_stream;
    
    int64_t _sequence_id;
    
    int _c_cpp_identifier;
    const void * _service;
    
    const void * get_service(const rosidl_service_type_support_t * service_type_support);
    const rosidl_service_type_support_t * get_service_type_support(const rosidl_service_type_support_t * type_supports);

};

#endif
