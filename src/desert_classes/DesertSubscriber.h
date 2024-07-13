#ifndef DESERT_SUBSCRIBER_H_
#define DESERT_SUBSCRIBER_H_

#include "CBorStream.h"

#include <vector>
#include <string>

class DesertSubscriber
{
  public:
    DesertSubscriber(std::string topic_name);
    
    bool has_data();
  
  private:
    cbor::RxStream _data_stream;
    std::string _name;

};

#endif
