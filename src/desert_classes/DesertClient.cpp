#include "DesertClient.h"

DesertClient::DesertClient(std::string service_name, const rosidl_service_type_support_t * type_supports)
      : _name(service_name)
      , _data_stream(cbor::TxStream())
{
  const rosidl_service_type_support_t * type_support = get_type_support(type_supports);
  _members = get_members(type_support);
}

void DesertClient::send_request(const void * req)
{
  printf("Sending a request\n");
}

const void * DesertClient::get_members(const rosidl_service_type_support_t * type_support)
{
  return type_support->data;
}

const rosidl_service_type_support_t * DesertClient::get_type_support(const rosidl_service_type_support_t * type_supports)
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
