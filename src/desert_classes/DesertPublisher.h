#ifndef DESERT_PUBLISHER_H_
#define DESERT_PUBLISHER_H_

#include "rosidl_typesupport_introspection_cpp/identifier.hpp"
#include "rosidl_typesupport_introspection_c/identifier.h"
#include "rosidl_typesupport_introspection_cpp/message_introspection.hpp"
#include "rosidl_typesupport_introspection_c/message_introspection.h"
#include "rosidl_typesupport_introspection_cpp/service_introspection.hpp"
#include "rosidl_typesupport_introspection_c/service_introspection.h"
#include "rosidl_typesupport_introspection_cpp/field_types.hpp"
#include "rosidl_typesupport_introspection_c/field_types.h"

#include "rosidl_typesupport_cpp/identifier.hpp"
#include "rosidl_typesupport_c/type_support_map.h"
#include "rosidl_typesupport_c/identifier.h"

#include "rosidl_runtime_c/message_type_support_struct.h"

#include "CBorStream.h"
#include "MessageSerialization.h"

#include <vector>
#include <string>

class DesertPublisher
{
  public:
    DesertPublisher(std::string topic_name, const rosidl_message_type_support_t * type_supports);
    
    void push(const void * msg);
    
    
  private:
    cbor::TxStream _data_stream;
    std::string _name;
    
    int _c_cpp_identifier;
    const void * _members;
    
    const void * get_members(const rosidl_message_type_support_t * type_support);
    const rosidl_message_type_support_t * get_type_support(const rosidl_message_type_support_t * type_supports);

};

#endif
