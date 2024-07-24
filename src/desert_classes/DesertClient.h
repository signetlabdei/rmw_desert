#ifndef DESERT_CLIENT_H_
#define DESERT_CLIENT_H_

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

#include "rosidl_runtime_c/service_type_support_struct.h"

#include "CBorStream.h"
#include "MessageSerialization.h"

#include <vector>
#include <string>

class DesertClient
{
  public:
    DesertClient(std::string service_name, const rosidl_service_type_support_t * type_supports);
    
    void send_request(const void * req, int64_t sequence_id);
    
    
  private:
    cbor::TxStream _request_data_stream;
    cbor::RxStream _response_data_stream;
    std::string _name;
    uint64_t _sequence_id;
    
    int _c_cpp_identifier;
    const void * _service;
    
    const void * get_service(const rosidl_service_type_support_t * service_type_support);
    const rosidl_service_type_support_t * get_service_type_support(const rosidl_service_type_support_t * type_supports);

};

#endif
