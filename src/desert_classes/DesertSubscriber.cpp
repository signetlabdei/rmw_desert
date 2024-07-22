#include "DesertSubscriber.h"

DesertSubscriber::DesertSubscriber(std::string topic_name, const rosidl_message_type_support_t * type_supports)
      : _name(topic_name)
      , _data_stream(cbor::RxStream(topic_name))
{
  const rosidl_message_type_support_t * type_support = get_type_support(type_supports);
  _members = get_members(type_support);
}

bool DesertSubscriber::has_data()
{
  cbor::RxStream::interpret_packets();
  return _data_stream.data_available();
}

void DesertSubscriber::read_data(void * msg)
{
  switch (_c_cpp_identifier)
  {
    case 0:
    {
      auto casted_members = static_cast<const INTROSPECTION_C_MEMBERS *>(_members);
      MessageSerialization::deserialize<INTROSPECTION_C_MEMBERS>(msg, casted_members, _data_stream);
      break;
    }
    case 1:
    {
      auto casted_members = static_cast<const INTROSPECTION_CPP_MEMBERS *>(_members);
      MessageSerialization::deserialize<INTROSPECTION_CPP_MEMBERS>(msg, casted_members, _data_stream);
      break;
    }
  }
}

const void * DesertSubscriber::get_members(const rosidl_message_type_support_t * type_support)
{
  return type_support->data;
}

const rosidl_message_type_support_t * DesertSubscriber::get_type_support(const rosidl_message_type_support_t * type_supports)
{
  const rosidl_message_type_support_t * ts;
  if ((ts = get_message_typesupport_handle(type_supports, rosidl_typesupport_introspection_c__identifier)) != nullptr)
  {
    _c_cpp_identifier = 0;
    return ts;
  }
  else 
  {
    rcutils_error_string_t prev_error_string = rcutils_get_error_string();
    rcutils_reset_error();
    if ((ts = get_message_typesupport_handle(type_supports, rosidl_typesupport_introspection_cpp::typesupport_identifier)) != nullptr)
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
