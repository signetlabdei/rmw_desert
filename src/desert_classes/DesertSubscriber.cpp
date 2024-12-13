#include "DesertSubscriber.h"

DesertSubscriber::DesertSubscriber(std::string topic_name, const rosidl_message_type_support_t * type_supports, rmw_gid_t gid)
      : _name(topic_name)
      , _id(TopicsConfig::get_topic_identifier(topic_name))
      , _data_stream(cbor::RxStream(SUBSCRIBER_TYPE, topic_name, _id))
      , _gid(gid)
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
  if (_c_cpp_identifier == 0)
  {
    auto casted_members = static_cast<const INTROSPECTION_C_MEMBERS *>(_members);
    MessageSerialization::deserialize(msg, casted_members, _data_stream);
  }
  else if (_c_cpp_identifier == 1)
  {
    auto casted_members = static_cast<const INTROSPECTION_CPP_MEMBERS *>(_members);
    MessageSerialization::deserialize(msg, casted_members, _data_stream);
  }
  
  _data_stream.clear_buffer();
}

rmw_gid_t DesertSubscriber::get_gid()
{
  return _gid;
}

std::string DesertSubscriber::get_topic_name()
{
  return _name;
}

std::string DesertSubscriber::get_type_name()
{
  std::string namespace_;
  std::string name_;
    
  if (_c_cpp_identifier == 0)
  {
    auto casted_members = static_cast<const INTROSPECTION_C_MEMBERS *>(_members);
    namespace_ = casted_members->message_namespace_;
    name_ = casted_members->message_name_;
  }
  else if (_c_cpp_identifier == 1)
  {
    auto casted_members = static_cast<const INTROSPECTION_CPP_MEMBERS *>(_members);
    namespace_ = casted_members->message_namespace_;
    name_ = casted_members->message_name_;
  }

  std::string namespace_sleshed = regex_replace(namespace_, std::regex("::"), "/");
  
  return namespace_sleshed + "/" + name_;
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
