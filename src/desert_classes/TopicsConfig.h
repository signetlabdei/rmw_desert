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
 * @file TopicsConfig.h
 * @brief Class used to store configurations
 * 
 * In order to prevent sending a string containing the topic name for each packet, 
 * a configuration file is used to associate each topic string to an integer that 
 * uses much less bandwidth and blocks topics that are not in this list.
 *
 * @author Prof. Davide Costa
 *
 */

#ifndef TOPICS_CONFIG_H_
#define TOPICS_CONFIG_H_

/** @cond */

#include <map>
#include <string>
#include <cstdint>
#include <fstream>

/** @endcond */

#include "json.hpp"

using namespace nlohmann::json_abi_v3_11_3;

class TopicsConfig
{
  public:
  
   /**
    * @brief Initialize the configuration
    *
    * This function reads the configuration file from ./ros_allowed_topics.conf. 
    * If not present, a warning will be displayed.
    */
    static void load_configuration();
   /**
    * @brief Get topic's identifier from configuration
    *
    * This function returns the identifier associated to a topic if it exists, 
    * otherwise returns zero.
    *
    * @return The topic identifier
    */
    static uint8_t get_topic_identifier(std::string name);
   /**
    * @brief Get identifier's topic from configuration
    *
    * This function returns the topic associated to an identifier if it exists, 
    * otherwise returns an empty string.
    *
    * @return The identifier topic
    */
    static std::string get_identifier_topic(uint8_t identifier);
  
  private:
    static std::map<std::string, uint8_t> _topics_list;
    static std::map<uint8_t, std::string> _identifiers_list;
};

#endif  // MACROS_HPP_
