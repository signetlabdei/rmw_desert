#ifndef DESERT_SUBSCRIBER_H_
#define DESERT_SUBSCRIBER_H_

#include "../classes.h"

#include <vector>
#include <string>

#define INTROSPECTION_C_MEMBER rosidl_typesupport_introspection_c__MessageMember
#define INTROSPECTION_CPP_MEMBER rosidl_typesupport_introspection_cpp::MessageMember

#define INTROSPECTION_C_MEMBERS rosidl_typesupport_introspection_c__MessageMembers
#define INTROSPECTION_CPP_MEMBERS rosidl_typesupport_introspection_cpp::MessageMembers

class DesertSubscriber
{
  public:
    DesertSubscriber(std::string topic_name, const rosidl_message_type_support_t * type_supports);
    
    bool has_data();
    void read_data(void * msg);
  
  private:
    cbor::RxStream _data_stream;
    std::string _name;
    
    int _c_cpp_identifier;
    const void * _members;
    
    const void * get_members(const rosidl_message_type_support_t * type_support);
    const rosidl_message_type_support_t * get_type_support(const rosidl_message_type_support_t * type_supports);
    
    template<typename MembersType>
    void deserialize(void * msg, const MembersType * casted_members);
    
    template<typename T>
    void deserialize_field(const INTROSPECTION_CPP_MEMBER * member, void * field);
    template<typename T>
    void deserialize_field(const INTROSPECTION_C_MEMBER * member, void * field);

};

#endif
