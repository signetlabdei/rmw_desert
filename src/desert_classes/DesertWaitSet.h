#ifndef DESERT_WAIT_SET_H_
#define DESERT_WAIT_SET_H_

class DesertWaitset
{
  public:
    DesertWaitset()
    {}
    
    std::mutex lock;
    bool inuse;
};

#endif
