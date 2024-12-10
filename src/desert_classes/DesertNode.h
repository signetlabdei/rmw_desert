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
 * @file DesertNode.h
 * @brief Implementation of the Node structure for DESERT
 * 
 * The DesertNode class is used to keep track af all the entities created by a specific node. 
 * Each of them is stored in a vector of pointers to the original memory locations mainly 
 * used to provide discovery functionalities.
 *
 * @author Prof. Davide Costa
 *
 */

#ifndef DESERT_NODE_H_
#define DESERT_NODE_H_

/** @cond */

#include "rmw/rmw.h"
#include "rmw/types.h"

#include <vector>
#include <string>

/** @endcond */

#include "CBorStream.h"
#include "DesertPublisher.h"
#include "DesertSubscriber.h"
#include "DesertClient.h"
#include "DesertService.h"
#include "Discovery.h"
#include "TopicsConfig.h"

class DesertNode
{
  public:
   /**
    * @brief Create a node
    *
    * @param name       Name of the node
    * @param namespace_ Namespace of the node
    * @param gid        Global identifier of the node
    */
    DesertNode(std::string name, std::string namespace_, rmw_gid_t gid);
    ~DesertNode();
    
   /**
    * @brief Add a publisher to the current node
    *
    * This function pushes the pointer to a publisher in a vector of all the registered 
    * publishers related to the current node.
    *
    * @param pub Pointer to a DesertPublisher instance
    */
    void add_publisher(DesertPublisher * pub);
    
   /**
    * @brief Add a subscriber to the current node
    *
    * This function pushes the pointer to a subscriber in a vector of all the registered 
    * subscribers related to the current node.
    *
    * @param sub Pointer to a DesertSubscriber instance
    */
    void add_subscriber(DesertSubscriber * sub);
    
   /**
    * @brief Add a client to the current node
    *
    * This function pushes the pointer to a client in a vector of all the registered 
    * clients related to the current node.
    *
    * @param cli Pointer to a DesertClient instance
    */
    void add_client(DesertClient * cli);
    
   /**
    * @brief Add a service to the current node
    *
    * This function pushes the pointer to a service in a vector of all the registered 
    * services related to the current node.
    *
    * @param ser Pointer to a DesertService instance
    */
    void add_service(DesertService * ser);
    
   /**
    * @brief Remove a publisher from the current node
    *
    * This function removes the pointer to a publisher from the vector of all the registered 
    * publishers related to the current node.
    *
    * @param pub Pointer to a DesertPublisher instance
    */
    void remove_publisher(DesertPublisher * pub);
    
   /**
    * @brief Remove a subscriber from the current node
    *
    * This function removes the pointer to a subscriber from the vector of all the registered 
    * subscribers related to the current node.
    *
    * @param sub Pointer to a DesertSubscriber instance
    */
    void remove_subscriber(DesertSubscriber * sub);
    
   /**
    * @brief Remove a client from the current node
    *
    * This function removes the pointer to a client from the vector of all the registered 
    * clients related to the current node.
    *
    * @param cli Pointer to a DesertClient instance
    */
    void remove_client(DesertClient * cli);
    
   /**
    * @brief Remove a service from the current node
    *
    * This function removes the pointer to a service from the vector of all the registered 
    * services related to the current node.
    *
    * @param ser Pointer to a DesertService instance
    */
    void remove_service(DesertService * ser);
    
   /**
    * @brief Retreive the gid of the current entity
    *
    * This function returns the global identifier of the current entity in the rmw format.
    *
    * @return Global identifier of the entity
    */
    rmw_gid_t get_gid();
    
  private:
    rmw_gid_t _gid;
    std::string _name;
    std::string _namespace;
    cbor::TxStream _discovery_beacon_data_stream;
    cbor::RxStream _discovery_request_data_stream;
    
    std::vector<DesertPublisher *> _publishers;
    std::vector<DesertSubscriber *> _subscribers;
    std::vector<DesertClient *> _clients;
    std::vector<DesertService *> _services;
    
    void publish_all_beacons();
    
    bool _discovery_done;
    std::thread _discovery_request_thread;
    
    void _discovery_request();
    
};

#endif
