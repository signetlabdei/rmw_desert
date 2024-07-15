#include "DesertSubscriber.h"

DesertSubscriber::DesertSubscriber(std::string topic_name)
      : _name(topic_name)
      , _data_stream(cbor::RxStream(topic_name))
{
}

bool DesertSubscriber::has_data()
{
  cbor::RxStream::interpret_packets();
  return _data_stream.data_available();
}

void * DesertSubscriber::read_data()
{
  std::vector<std::pair<void *, int>> packet;
  _data_stream >> packet;
  if (packet.size() > 0 && packet[0].second == CBOR_ITEM_STRING)
    printf("SE VA: %s\n", static_cast<std::string *>(packet[0].first)->c_str());
  return nullptr;
}
