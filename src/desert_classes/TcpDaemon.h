#ifndef TCP_DAEMON_H_
#define TCP_DAEMON_H_

#include <queue>
#include <vector>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <thread>
#include <chrono>

#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

#include "rmw/error_handling.h"

#define ADDRESS "192.168.1.110"
#define PORT 4000

class TcpDaemon
{
  public:
    TcpDaemon();
    
    bool init();
    static void enqueue_packet(std::vector<uint8_t> packet);
    
    
  private:
    static int _client_fd;
    static std::queue<std::vector<uint8_t>> _rx_packets;
    static std::queue<std::vector<uint8_t>> _tx_packets;
    
    void socket_communication();

};

#endif
