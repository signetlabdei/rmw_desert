#include <mutex>

class DesertNode
{
  public:
    DesertNode(const char* name)
    : _name(name)
    {}
    
    const char * getName()
    {
      return _name;
    }
  private:
    const char * _name;
};

class DesertPublisher
{
  public:
    DesertPublisher(const char* topic_name, uint64_t id)
      : _name(topic_name)
      , _id(id)
    {}
    
    void push(const void* msg)
    {
    }

  private:
    const char* _name;
    uint64_t _id;

};

class DesertSubscriber
{
  public:
    DesertSubscriber(const char * topic_name)
      : _topic_name(topic_name)
    {}
  
  private:
    const char * _topic_name;

};

class DesertWaitset
{
  public:
    DesertWaitset()
    {}
    
    std::mutex lock;
    bool inuse;
};
