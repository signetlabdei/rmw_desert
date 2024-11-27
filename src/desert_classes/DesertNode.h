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
 * Unimplemented class included for future expansions
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

#include "CBorStream.h"
#include "DesertPublisher.h"
#include "DesertSubscriber.h"
#include "DesertClient.h"
#include "DesertService.h"
#include "Discovery.h"
#include "TopicsConfig.h"

/** @endcond */

class DesertNode
{
  public:
    DesertNode(std::string name, std::string namespace_, rmw_gid_t gid);
    ~DesertNode();
    
    void add_publisher(DesertPublisher * pub);
    void add_subscriber(DesertSubscriber * sub);
    void add_client(DesertClient * cli);
    void add_service(DesertService * ser);
    
    void remove_publisher(DesertPublisher * pub);
    void remove_subscriber(DesertSubscriber * sub);
    void remove_client(DesertClient * cli);
    void remove_service(DesertService * ser);
    
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
