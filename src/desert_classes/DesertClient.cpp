#include "DesertClient.h"

DesertClient::DesertClient(std::string service_name, const rosidl_service_type_support_t * type_supports)
      : _name(service_name)
      , _request_data_stream(cbor::TxStream(CLIENT_TYPE, service_name))
      , _response_data_stream(cbor::RxStream(CLIENT_TYPE, service_name))
{
  const rosidl_service_type_support_t * service_type_support = get_service_type_support(type_supports);
  _service = get_service(service_type_support);
}

void DesertClient::send_request(const void * req, int64_t sequence_id)
{
  _sequence_id = sequence_id;
  printf("Sending a request with id %d\n", sequence_id);
  
  _request_data_stream.start_transmission(sequence_id);
  
  if (_c_cpp_identifier == 0)
  {
    auto casted_service = static_cast<const INTROSPECTION_C_SERVICE_MEMBERS *>(_service);
    MessageSerialization::serialize<INTROSPECTION_C_MEMBERS>(req, casted_service->request_members_, _request_data_stream);
  }
  else if (_c_cpp_identifier == 1)
  {
    auto casted_service = static_cast<const INTROSPECTION_CPP_SERVICE_MEMBERS *>(_service);
    MessageSerialization::serialize<INTROSPECTION_CPP_MEMBERS>(req, casted_service->request_members_, _request_data_stream);
  }
  
  _request_data_stream.end_transmission();
}

const void * DesertClient::get_service(const rosidl_service_type_support_t * service_type_support)
{
  return service_type_support->data;
}

const rosidl_service_type_support_t * DesertClient::get_service_type_support(const rosidl_service_type_support_t * type_supports)
{
  const rosidl_service_type_support_t * ts;
  if ((ts = get_service_typesupport_handle(type_supports, rosidl_typesupport_introspection_c__identifier)) != nullptr)
  {
    _c_cpp_identifier = 0;
    return ts;
  }
  else 
  {
    rcutils_error_string_t prev_error_string = rcutils_get_error_string();
    rcutils_reset_error();
    if ((ts = get_service_typesupport_handle(type_supports, rosidl_typesupport_introspection_cpp::typesupport_identifier)) != nullptr)
    {
      _c_cpp_identifier = 1;
      return ts;
    }
    else
    {
      rcutils_error_string_t error_string = rcutils_get_error_string();
      rcutils_reset_error();
      RMW_SET_ERROR_MSG_WITH_FORMAT_STRING(
        "Type support not from this implementation. Got:\n"
        "    %s\n"
        "    %s\n"
        "while fetching it",
        prev_error_string.str, error_string.str);
      return nullptr;
    }
  }
};
