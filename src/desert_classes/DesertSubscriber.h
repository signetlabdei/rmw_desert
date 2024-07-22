#ifndef DESERT_SUBSCRIBER_H_
#define DESERT_SUBSCRIBER_H_

#include "rosidl_typesupport_introspection_cpp/identifier.hpp"
#include "rosidl_typesupport_introspection_c/identifier.h"
#include "rosidl_typesupport_introspection_cpp/message_introspection.hpp"
#include "rosidl_typesupport_introspection_c/message_introspection.h"
#include "rosidl_typesupport_introspection_cpp/field_types.hpp"
#include "rosidl_typesupport_introspection_c/field_types.h"

#include "rosidl_typesupport_cpp/identifier.hpp"
#include "rosidl_typesupport_c/type_support_map.h"
#include "rosidl_typesupport_c/identifier.h"

#include "CBorStream.h"
#include "MessageSerialization.h"

#include <vector>
#include <string>

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

};

#endif
