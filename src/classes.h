class Node
{
  public:
    Node(const char* name)
    : _name(name)
    {}
    
    const char * getName()
    {
      return _name;
    }
  private:
    const char * _name;
};

class Publisher
{
  public:
    Publisher(const char* topic_name, /*serialize_func_t serialize, */uint64_t id)
      : _name(topic_name)
      , _id(id)
      , _seq_num(0)
      , _req_seq_num(0)
      /*, _serialize(serialize)
      , _heartbeat("publisher", topic_name, 0)*/
    {
      //requestSync();
    }
    
    //typedef size_t (*serialize_func_t)(const void* ros_message, char* buffer, size_t buffer_size);
    
    void push(const void* msg)
    {
    }

  private:
    const char* _name;
    uint64_t _id;
    uint64_t _seq_num;
    uint64_t _req_seq_num;
    //serialize_func_t _serialize;
    //std::vector<ndn::Data> _queue;
    //DiscoveryHeartbeatEmiter _heartbeat;
    //SyncPublisher _sync;
    //TopicPublisher _topic;
    
    void requestSync()
    {
    }

    void onUsedId(/*const ndn::Data& data*/)
    {
    }

    void onFreeId(/*const ndn::Interest& interest*/)
    {
    }

};
