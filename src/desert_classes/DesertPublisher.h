#ifndef DESERT_PUBLISHER_H_
#define DESERT_PUBLISHER_H_

#include "../classes.h"

class DesertPublisher
{
  public:
    DesertPublisher(const char * topic_name, uint64_t id, const rosidl_message_type_support_t * type_supports);
    
    void push(const void * msg);
    
    
  private:
    const char * _name;
    uint64_t _id;
    
    int _c_cpp_identifier;
    const rosidl_typesupport_introspection_c__MessageMembers * _c_members;
    const rosidl_typesupport_introspection_cpp::MessageMembers * _cpp_members;
    
    void set_members(const rosidl_message_type_support_t * type_support);
    const rosidl_message_type_support_t * get_type_support(const rosidl_message_type_support_t * type_supports);

};

#endif
