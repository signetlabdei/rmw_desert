#ifndef DESERT_NODE_H_
#define DESERT_NODE_H_

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

#endif
