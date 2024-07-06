#include "DesertPublisher.h"

DesertPublisher::DesertPublisher(const char* topic_name, uint64_t id, const rosidl_message_type_support_t * type_supports)
      : _name(topic_name)
      , _id(id)
{
  const rosidl_message_type_support_t * type_support = get_type_support(type_supports);
  _members = get_members(type_support);
}

void DesertPublisher::push(const void * msg)
{
  switch (_c_cpp_identifier)
  {
    case 0:
    {
      auto casted_members = static_cast<const rosidl_typesupport_introspection_c__MessageMembers *>(_members);
      push<rosidl_typesupport_introspection_c__MessageMembers>(msg, casted_members);
      break;
    }
    case 1:
    {
      auto casted_members = static_cast<const rosidl_typesupport_introspection_cpp::MessageMembers *>(_members);
      push<rosidl_typesupport_introspection_cpp::MessageMembers>(msg, casted_members);
      break;
    }
  }
}

template<typename MembersType>
void DesertPublisher::push(const void * msg, const MembersType * casted_members)
{
  if (casted_members->members_[0].type_id_ == ::rosidl_typesupport_introspection_cpp::ROS_TYPE_STRING) {
    auto c_string = static_cast<const rosidl_runtime_c__String *>(msg);
    printf("INCOMING MESSAGE: %s\n", std::string(c_string->data).c_str());
  }
}

const void * DesertPublisher::get_members(const rosidl_message_type_support_t * type_support)
{
  return type_support->data;
}

const rosidl_message_type_support_t * DesertPublisher::get_type_support(const rosidl_message_type_support_t * type_supports)
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
