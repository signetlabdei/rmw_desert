#include "TcpDaemon.h"

TcpDaemon::TcpDaemon()
{
}

int TcpDaemon::_client_fd;
std::queue<std::vector<uint8_t>> TcpDaemon::_rx_packets;
std::queue<std::vector<uint8_t>> TcpDaemon::_tx_packets;

bool TcpDaemon::init()
{
  int status, valread;
  struct sockaddr_in serv_addr;

  if ((_client_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    RMW_SET_ERROR_MSG("Socket creation error");
    return -1;
  }

  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(PORT);

  // Convert IPv4 and IPv6 addresses from text to binary form
  if (inet_pton(AF_INET, ADDRESS, &serv_addr.sin_addr) <= 0) {
    RMW_SET_ERROR_MSG("Invalid address / Address not supported");
    return -1;
  }

  if ((status = connect(_client_fd, (struct sockaddr*)&serv_addr, sizeof(serv_addr))) < 0) {
    RMW_SET_ERROR_MSG("Connection to DESERT socket failed");
    return -1;
  }
  
  // Start the packet transceiver thread
  std::thread daemon = std::thread(&TcpDaemon::socket_communication, this);
  daemon.detach();
  
  return 0;
}

void TcpDaemon::enqueue_packet(std::vector<uint8_t> packet)
{
  _tx_packets.push(packet);
}

void TcpDaemon::socket_communication()
{
  while (true)
  {
    for(size_t i=0; i < _tx_packets.size(); i++)
    {
      std::vector<uint8_t> packet = _tx_packets.front();
      const char * casted_packet = reinterpret_cast<const char*>(packet.data());
      send(_client_fd, casted_packet, strlen(casted_packet), 0);
      _tx_packets.pop();
    }
    
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
  }
}
