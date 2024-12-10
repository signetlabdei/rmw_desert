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
 * @file demangle.h
 * @brief Functions used to demangle topic names during discovery operations
 * 
 * Demangle functions allows to extract topic names and type names of 
 * each entity stored in the common context implementation. Since in 
 * this object the informations are divided in writers and readers, 
 * they must be converted in publishers, subscribers, clients and
 * services.
 *
 * @author Prof. Davide Costa
 *
 */

#ifndef DEMANGLE_H_
#define DEMANGLE_H_

/** @cond */

#include <algorithm>
#include <string>
#include <vector>

#include "rcpputils/find_and_replace.hpp"
#include "rcutils/logging_macros.h"
#include "rcutils/types.h"

/** @endcond */

#include "CBorStream.h"

/**
 * @namespace Discovery
 * @brief Namespace containing discovery functions
 * 
 * The middleware layer of a ROS stack must implement functionalities 
 * used to inform each node of the network structure of the other nodes 
 * connected, with their names and topics. Since this operation is 
 * quite resource-consuming and the underwater channel has a limited 
 * bandwidth, it is possible to disable it.
 */
namespace Discovery
{
  
  char * integer_to_string(int x);

  const char * const ros_topic_publisher_prefix = integer_to_string(PUBLISHER_TYPE);
  const char * const ros_topic_subscriber_prefix = integer_to_string(SUBSCRIBER_TYPE);
  const char * const ros_service_requester_prefix = integer_to_string(CLIENT_TYPE);
  const char * const ros_service_response_prefix = integer_to_string(SERVICE_TYPE);
  
 /**
  * @brief Resolve a prefix
  *
  * Returns `name` stripped of `prefix`.
  *
  * @param name   Mangled topic name
  * @param prefix Prefix of the entity type
  * @return       Demangled topic name
  */
  std::string resolve_prefix(const std::string & name, const std::string & prefix);

 /**
  * @brief Demangle a publisher
  *
  * Return the topic name for a given topic if it is part of a publisher, else "".
  *
  * @param topic_name Mangled topic name
  * @return           Demangled topic name
  */
  std::string demangle_publisher_from_topic(const std::string & topic_name);

 /**
  * @brief Demangle a subscriber
  *
  * Return the topic name for a given topic if it is part of a subscriber, else "".
  *
  * @param topic_name Mangled topic name
  * @return           Demangled topic name
  */
  std::string demangle_subscriber_from_topic(const std::string & topic_name);

 /**
  * @brief Demangle a topic
  *
  * Return the topic name for a given topic if it is part of one, else "".
  *
  * @param topic_name Mangled topic name
  * @return           Demangled topic name
  */
  std::string demangle_topic(const std::string & topic_name);

 /**
  * @brief Demangle a service request
  *
  * Return the service name for a given topic if it is part of a service request, else "".
  *
  * @param topic_name Mangled topic name
  * @return           Demangled topic name
  */
  std::string demangle_service_request_from_topic(const std::string & topic_name);

 /**
  * @brief Demangle a service reply
  *
  * Return the service name for a given topic if it is part of a service reply, else "".
  *
  * @param topic_name Mangled topic name
  * @return           Demangled topic name
  */
  std::string demangle_service_reply_from_topic(const std::string & topic_name);

 /**
  * @brief Demangle a service
  *
  * Return the service name for a given topic if it is part of a service, else "".
  *
  * @param topic_name Mangled topic name
  * @return           Demangled topic name
  */
  std::string demangle_service_from_topic(const std::string & topic_name);

 /**
  * @brief No demangle
  *
  * Used when ros names are not mangled.
  *
  * @param name Topic name
  * @return     Same topic name
  */
  std::string identity_demangle(const std::string & name);

  using DemangleFunction = std::string (*)(const std::string &);

}

#endif  // DEMANGLE_H_
