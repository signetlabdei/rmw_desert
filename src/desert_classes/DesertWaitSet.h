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
 * @file DesertWaitSet.h
 * @brief Implementation of the WaitSet structure for DESERT
 * 
 * Unimplemented class included for future expansions
 *
 * @author Prof. Davide Costa
 *
 */

#ifndef DESERT_WAIT_SET_H_
#define DESERT_WAIT_SET_H_

class DesertWaitset
{
  public:
    DesertWaitset()
    {}
    
    std::mutex lock;
    bool inuse;
};

#endif
