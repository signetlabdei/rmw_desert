#include "TcpDaemon.h"

TcpDaemon::TcpDaemon()
{
}

std::queue<std::vector<uint8_t>> TcpDaemon::rx_packets;
std::queue<std::vector<uint8_t>> TcpDaemon::tx_packets;

void TcpDaemon::enqueue_packet(std::vector<uint8_t> packet)
{
  tx_packets.push(packet);
  for(size_t i=0; i < packet.size(); i++)
    printf("%02x ", packet[i]);
  printf("\n");
}
