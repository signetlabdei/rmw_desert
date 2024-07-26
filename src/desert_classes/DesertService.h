#ifndef DESERT_SERVICE_H_
#define DESERT_SERVICE_H_

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

#include "rmw/types.h"

#include "CBorStream.h"
#include "MessageSerialization.h"

#include <vector>
#include <string>

class DesertService
{
  public:
    DesertService(std::string service_name, const rosidl_service_type_support_t * type_supports);
    
    bool has_data();
    void read_request(void * req, rmw_service_info_t * req_header);
    void send_response(void * res, rmw_request_id_t * req_header);
    
    
  private:
    cbor::RxStream _request_data_stream;
    cbor::TxStream _response_data_stream;
    std::string _name;
    int64_t _sequence_id;
    
    int _c_cpp_identifier;
    const void * _service;
    
    const void * get_service(const rosidl_service_type_support_t * service_type_support);
    const rosidl_service_type_support_t * get_service_type_support(const rosidl_service_type_support_t * type_supports);

};

#endif
