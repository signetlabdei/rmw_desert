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
 * @file TcpDaemon.h
 * @brief Class used to send and receive data from the DESERT socket
 * 
 * The DESERT protocol stack interacts with the application level through a socket,
 * used to send and receive a binary stream containing packets. This class connects
 * to the socket and creates two threads, that run continously to store and send packets
 * in the static members rx_packets and tx_packets
 *
 * @author Prof. Davide Costa
 *
 */

#ifndef TCP_DAEMON_H_
#define TCP_DAEMON_H_

/** @cond */

#include <queue>
#include <vector>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <thread>
#include <chrono>

#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/poll.h>
#include <unistd.h>

#include "rmw/error_handling.h"

/** @endcond */

#define MAX_PACKET_LENGTH 512

#define ADDRESS "127.0.0.1"

#define START_MARKER 0b10011001
#define END_MARKER   0b01010101
#define BYTE_MASK    0b11111111

class TcpDaemon
{
  public:
    TcpDaemon();
    
   /**
    * @brief Initialize the socket communication
    *
    * This function allows the middleware to estabilish a connection to the
    * DESERT stack through a TCP socket.
    *
    * @param port The TCP port of the DESERT application layer
    */
    bool init(int port);
   /**
    * @brief Read a packet from the _rx_packets member as vector of bytes
    *
    * This function is used by the various RxStream instances contained in 
    * subscribers, clients and services.
    *
    * @return The packet that was read from the DESERT stack
    */
    static std::vector<uint8_t> read_packet();
   /**
    * @brief Enqueue a packet in the _tx_packets member as vector of bytes
    *
    * This function is used by the various TxStream instances contained in 
    * publishers, clients and services.
    *
    * @param packet The packet that has to be sent through the DESERT stack
    */
    static void enqueue_packet(std::vector<uint8_t> packet);
    
    
  private:
    static int _client_fd;
    static std::queue<std::vector<uint8_t>> _rx_packets;
    static std::queue<std::vector<uint8_t>> _tx_packets;
    
    void socket_rx_communication();
    void socket_tx_communication();

};

#endif
