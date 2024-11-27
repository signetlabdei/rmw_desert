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
 * @file rmw_context_impl_s.h
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
 
#include "rcpputils/scope_exit.hpp"
#include "rmw_dds_common/context.hpp"
#include "rmw_dds_common/graph_cache.hpp"
#include "rmw_dds_common/msg/participant_entities_info.hpp"
#include "rmw_dds_common/qos.hpp"
#include "rmw_dds_common/security.hpp"

#ifndef RMW_CONTEXT_IMPL_H_
#define RMW_CONTEXT_IMPL_H_

struct rmw_context_impl_s
{
  rmw_dds_common::Context common;
  
  rmw_context_impl_s()
  : common()
  {
    /* destructor relies on these being initialized properly */
    common.thread_is_running.store(false);
    common.graph_guard_condition = nullptr;
    common.pub = nullptr;
    common.sub = nullptr;
  }

  // Initializes the participant, if it wasn't done already.
  rmw_ret_t init(rmw_init_options_t * options, size_t domain_id);

  // Destroys the participant, when node_count reaches 0.
  rmw_ret_t fini()
  {
  }

  ~rmw_context_impl_s()
  {
  }

private:
  inline void clean_up()
  {
  }
};

#endif
