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
 * @brief -
 * 
 * -
 * -
 * -
 * -
 * -
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

namespace Discovery
{

  void discovery_thread(rmw_context_impl_t * impl);
  rmw_ret_t discovery_thread_start(rmw_context_impl_t * impl);
  
  void send_discovery_beacon(cbor::TxStream stream, std::string node_name, std::string node_namespace, int entity_type, rmw_gid_t entity_gid, std::string topic_name, std::string type_name, bool disconnect);
  void send_discovery_request(cbor::TxStream stream);

}

#endif
