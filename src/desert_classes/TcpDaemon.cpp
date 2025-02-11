#include "TcpDaemon.h"

TcpDaemon::TcpDaemon()
{
}

int TcpDaemon::_client_fd;
std::queue<std::vector<uint8_t>> TcpDaemon::_rx_packets;
std::queue<std::vector<uint8_t>> TcpDaemon::_tx_packets;

bool TcpDaemon::init(int port)
{
  int status, valread;
  struct sockaddr_in serv_addr;

  if ((_client_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
  {
    RMW_SET_ERROR_MSG("Socket creation error");
    return -1;
  }

  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(port);

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
  _tx_packets.push(packet);
  // Push also to _rx_packets to allow different nodes in the same applications to exchange data
  _rx_packets.push(packet);
}

void TcpDaemon::socket_rx_communication()
{
  std::vector<uint8_t> packet;
  bool found_header = false;
  uint8_t dimension_in_header = 0;
  
  while (true)
  {
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    
    char buffer[4086];
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
        
        // Found header
        if (!found_header && packet.end()[-2] == START_MARKER && packet.end()[-3] == START_MARKER)
        {
          packet.clear();
          found_header = true;
          dimension_in_header = field;
        }
        
        // Found tail
        if (found_header && packet.end()[-1] == END_MARKER)
        {
          uint8_t real_size = (packet.size() - 1) & BYTE_MASK;
          if (dimension_in_header == real_size)
          {
            packet.pop_back();
            _rx_packets.push(packet);
            found_header = false;
            packet.clear();
          }
        }
        
        // Erase oldest element if a RMW_desert header was not found
        if (!found_header && packet.size() > 5)
        {
          packet.erase(packet.begin());
        }
        
        // Overflow
        if (packet.size() > MAX_PACKET_LENGTH)
        {
          packet.clear();
          found_header = false;
        }
      }
          
      ptr += rc;
      poll(&fd, 1, 0);
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
      
      // Header
      uint8_t payload_size = packet.size() & BYTE_MASK;
      
      packet.insert(packet.begin(), payload_size);
      packet.insert(packet.begin(), START_MARKER);
      packet.insert(packet.begin(), START_MARKER);
      
      // Tail
      packet.push_back(END_MARKER);
    
      const char * casted_packet = reinterpret_cast<const char *>(packet.data());
      send(_client_fd, casted_packet, packet.size(), 0);
      _tx_packets.pop();
    }
  }
}
