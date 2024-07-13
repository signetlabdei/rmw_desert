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

  if ((_client_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
  {
    RMW_SET_ERROR_MSG("Socket creation error");
    return -1;
  }

  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(PORT);

  // Convert IPv4 and IPv6 addresses from text to binary form
  if (inet_pton(AF_INET, ADDRESS, &serv_addr.sin_addr) <= 0)
  {
    RMW_SET_ERROR_MSG("Invalid address / Address not supported");
    return -1;
  }

  if ((status = connect(_client_fd, (struct sockaddr*)&serv_addr, sizeof(serv_addr))) < 0)
  {
    RMW_SET_ERROR_MSG("Connection to DESERT socket failed");
    return -1;
  }
  
  // Start the packet transceiver threads
  std::thread rx_daemon = std::thread(&TcpDaemon::socket_rx_communication, this);
  std::thread tx_daemon = std::thread(&TcpDaemon::socket_tx_communication, this);
  rx_daemon.detach();
  tx_daemon.detach();
  
  return 0;
}

std::vector<uint8_t> TcpDaemon::read_packet()
{
  std::vector<uint8_t> packet;
  if (!_rx_packets.empty())
  {
    packet = _rx_packets.front();
    _rx_packets.pop();
  }
  return packet;
}

void TcpDaemon::enqueue_packet(std::vector<uint8_t> packet)
{
  uint8_t end_marker = 0b01010101;
  packet.push_back(end_marker);
  packet.push_back(end_marker);
  packet.push_back(end_marker);
    

  _tx_packets.push(packet);
}

void TcpDaemon::socket_rx_communication()
{
  std::vector<uint8_t> packet;
  uint8_t end_marker = 0b01010101;
  
  while (true)
  {
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    
    char buffer[1024];
    int ptr = 0;
    ssize_t rc;

    struct pollfd fd = {
      .fd = _client_fd,
      .events = POLLIN
    };

    poll(&fd, 1, 0); // Doesn't wait for data to arrive.
    while ( fd.revents & POLLIN )
    {
      rc = read(_client_fd, buffer + ptr, sizeof(buffer) - ptr);

      if ( rc <= 0 )
        break;

      for (int i=0; i < rc; i++)
      {
        uint8_t field = static_cast<uint8_t>(* (buffer + i));
        packet.push_back(field);

        bool found_end_sequence = true;
        for (int c=1; c <= 3; c++)
        {
          if (packet.end()[-c] != end_marker)
          {
            found_end_sequence = false;
          }
        }
        if (found_end_sequence)
        {
          packet.pop_back();
          packet.pop_back();
          packet.pop_back();
          _rx_packets.push(packet);
          packet.clear();
        }
      }
          
      ptr += rc;
      poll(&fd, 1, 0);
    }

    if (ptr != 0)
    {
      printf("Received %i bytes\n", ptr);
    }
  }
}

void TcpDaemon::socket_tx_communication()
{
  while (true)
  {
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    
    for(size_t i=0; i < _tx_packets.size(); i++)
    {
      std::vector<uint8_t> packet = _tx_packets.front();
      const char * casted_packet = reinterpret_cast<const char *>(packet.data());
      send(_client_fd, casted_packet, packet.size(), 0);
      _tx_packets.pop();
    }
  }
}
