// Copyright 2019 Open Source Robotics Foundation, Inc.
// Copyright 2016-2018 Proyectos y Sistemas de Mantenimiento SL (eProsima).
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef DEMANGLE_H_
#define DEMANGLE_H_

#include <algorithm>
#include <string>
#include <vector>

#include "rcpputils/find_and_replace.hpp"
#include "rcutils/logging_macros.h"
#include "rcutils/types.h"

#include "CBorStream.h"

namespace Discovery
{
  
  char * integer_to_string(int x);

  const char * const ros_topic_publisher_prefix = integer_to_string(PUBLISHER_TYPE);
  const char * const ros_topic_subscriber_prefix = integer_to_string(SUBSCRIBER_TYPE);
  const char * const ros_service_requester_prefix = integer_to_string(CLIENT_TYPE);
  const char * const ros_service_response_prefix = integer_to_string(SERVICE_TYPE);

  /// Returns `name` stripped of `prefix`.
  std::string resolve_prefix(const std::string & name, const std::string & prefix);

  /// Return the topic name for a given topic if it is part of one, else "".
  std::string demangle_publisher_from_topic(const std::string & topic_name);

  /// Return the topic name for a given topic if it is part of one, else "".
  std::string demangle_subscriber_from_topic(const std::string & topic_name);

  /// Return the topic name for a given topic if it is part of one, else "".
  std::string demangle_topic(const std::string & topic_name);

  /// Return the service name for a given topic if it is part of a service request, else "".
  std::string demangle_service_request_from_topic(const std::string & topic_name);

  /// Return the service name for a given topic if it is part of a service reply, else "".
  std::string demangle_service_reply_from_topic(const std::string & topic_name);

  /// Return the service name for a given topic if it is part of a service, else "".
  std::string demangle_service_from_topic(const std::string & topic_name);

  /// Used when ros names are not mangled.
  std::string identity_demangle(const std::string & name);

  using DemangleFunction = std::string (*)(const std::string &);

}

#endif  // DEMANGLE_H_
