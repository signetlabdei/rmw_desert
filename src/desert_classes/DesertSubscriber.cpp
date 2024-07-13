#include "DesertSubscriber.h"

DesertSubscriber::DesertSubscriber(std::string topic_name)
      : _name(topic_name)
      , _data_stream(cbor::RxStream(topic_name))
{
}

bool DesertSubscriber::has_data()
{
  cbor::RxStream::defragment_packets();
  return false;
}
