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
 * @file DesertGuardCondition.h
 * @brief Implementation of the GuardCondition structure for DESERT
 * 
 * The DesertGuardCondition class is used to handle trigger signals sent from rclcpp 
 * in order to break rcl_wait, usually when dealing with multithreading executors.
 *
 * @author Prof. Davide Costa
 *
 */

#ifndef DESERT_GUARD_CONDITION_H_
#define DESERT_GUARD_CONDITION_H_

/** @cond */

#include <array>
#include <atomic>
#include <cassert>
#include <condition_variable>
#include <mutex>
#include <utility>

#include "rcpputils/thread_safety_annotations.hpp"

/** @endcond */

class DesertGuardCondition
{
  public:
   /**
    * @brief Create a guard condition
    */
    DesertGuardCondition();
    
   /**
    * @brief Trigger the guard condition
    *
    * The trigger function sets the status of the atomic bool variable _has_triggered to save the 
    * new status of the guard condition.
    */
    void trigger();
    
   /**
    * @brief Check if the guard condition has triggered
    *
    * The has_triggered function returns a bool value with the status of the guard condition. Its 
    * internal state is not modified.
    *
    * @return True if the guard condition has triggered otherwise false
    */
    bool has_triggered();
    
   /**
    * @brief Check if the guard condition has triggered
    *
    * The get_has_triggered function returns a bool value with the status of the guard condition 
    * and resets the internal state to false.
    *
    * @return True if the guard condition has triggered otherwise false
    */
    bool get_has_triggered();
    
  private:
    std::atomic_bool _has_triggered;
};

#endif
