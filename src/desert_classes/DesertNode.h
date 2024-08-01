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

class DesertNode
{
  public:
    DesertNode(const char* name)
    : _name(name)
    {}
    
    const char * getName()
    {
      return _name;
    }
  private:
    const char * _name;
};

#endif
