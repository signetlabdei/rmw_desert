#ifndef TCP_DAEMON_H_
#define TCP_DAEMON_H_

#include <queue>
#include <vector>
#include <cstdint>
#include <cstdio>

class TcpDaemon
{
  public:
    TcpDaemon();
    
    static void enqueue_packet(std::vector<uint8_t> packet);
    
    
  private:
    static std::queue<std::vector<uint8_t>> rx_packets;
    static std::queue<std::vector<uint8_t>> tx_packets;

};

#endif
