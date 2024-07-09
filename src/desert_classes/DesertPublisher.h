#ifndef DESERT_PUBLISHER_H_
#define DESERT_PUBLISHER_H_

#include "../classes.h"
#include <vector>
#include <string>

#define INTROSPECTION_C_MEMBER rosidl_typesupport_introspection_c__MessageMember
#define INTROSPECTION_CPP_MEMBER rosidl_typesupport_introspection_cpp::MessageMember

#define INTROSPECTION_C_MEMBERS rosidl_typesupport_introspection_c__MessageMembers
#define INTROSPECTION_CPP_MEMBERS rosidl_typesupport_introspection_cpp::MessageMembers

class DesertPublisher
{
  public:
    DesertPublisher(std::string topic_name, uint64_t id, const rosidl_message_type_support_t * type_supports);
    
    void push(const void * msg);
    
    
  private:
    cbor::TxStream _data_stream;
    std::string _name;
    uint64_t _id;
    
    int _c_cpp_identifier;
    const void * _members;
    
    const void * get_members(const rosidl_message_type_support_t * type_support);
    const rosidl_message_type_support_t * get_type_support(const rosidl_message_type_support_t * type_supports);
    
    template<typename MembersType>
    void serialize(const void * msg, const MembersType * casted_members);
    
    template<typename T>
    void serialize_field(const INTROSPECTION_CPP_MEMBER * member, void * field);
    template<typename T>
    void serialize_field(const INTROSPECTION_C_MEMBER * member, void * field);

};

#endif
