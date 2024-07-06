#ifndef DESERT_SUBSCRIBER_H_
#define DESERT_SUBSCRIBER_H_

class DesertSubscriber
{
  public:
    DesertSubscriber(const char * topic_name)
      : _topic_name(topic_name)
    {}
  
  private:
    const char * _topic_name;

};

#endif
