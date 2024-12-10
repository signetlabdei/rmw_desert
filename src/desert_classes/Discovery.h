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
 * @file Discovery.h
 * @brief Namespace used to provide discovery functionalities
 * 
 * The middleware layer of a ROS stack must implement functionalities 
 * used to inform each node of the network structure of the other nodes 
 * connected, with their names and topics. Since this operation is 
 * quite resource-consuming and the underwater channel has a limited 
 * bandwidth, it is possible to disable it.
 *
 * @author Prof. Davide Costa
 *
 */
 
#include <thread>
#include <chrono>

#include "CBorStream.h"

#include "rmw/types.h"
#include "rmw/error_handling.h"

#include "rmw_context_impl_s.h"

#include "demangle.h"

#ifndef DISCOVERY_H_
#define DISCOVERY_H_

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
    
 /**
  * @brief Thread handling discovery beacons
  *
  * This function allows the middleware to receive and process incoming 
  * discovery beacons from the other nodes.
  *
  * @param impl The middleware comtext implementation
  */
  void discovery_thread(rmw_context_impl_t * impl);
    
 /**
  * @brief Initialize the discovery thread
  *
  * This function is called during the creation of a node and starts 
  * a discovery thread.
  *
  * @param impl The middleware comtext implementation
  * @return     Outcome of the operation
  */
  rmw_ret_t discovery_thread_start(rmw_context_impl_t * impl);
  
 /**
  * @brief Send a discovery beacon
  *
  * This function sends a beacon in the underwater channel containing 
  * all the informations related to a specific entity of a node.
  *
  * @param stream         The stream used to send data
  * @param node_name      The name of the node holding the entity
  * @param node_namespace The namespace of the node holding the entity
  * @param entity_type    The type of the entity
  * @param entity_gid     The global identifier of the entity
  * @param topic_name     The topic name
  * @param type_name      The topic type
  * @param disconnect     Flag used to determine if an entity is connecting or disconnecting
  */
  void send_discovery_beacon(cbor::TxStream stream, std::string node_name, std::string node_namespace, int entity_type, rmw_gid_t entity_gid, std::string topic_name, std::string type_name, bool disconnect);
  
 /**
  * @brief Send a discovery request
  *
  * This function sends a request in the underwater channel to all 
  * the nodes requiring them to send their discovery beacons.
  *
  * @param stream The stream used to send data
  */
  void send_discovery_request(cbor::TxStream stream);

}

#endif
