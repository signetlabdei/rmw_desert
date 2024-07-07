#include "DesertPublisher.h"

DesertPublisher::DesertPublisher(const char* topic_name, uint64_t id, const rosidl_message_type_support_t * type_supports)
      : _name(topic_name)
      , _id(id)
{
  _data_stream = cbor::TxStream();
  const rosidl_message_type_support_t * type_support = get_type_support(type_supports);
  _members = get_members(type_support);
}

void DesertPublisher::push(const void * msg)
{
  switch (_c_cpp_identifier)
  {
    case 0:
    {
      auto casted_members = static_cast<const INTROSPECTION_C_MEMBERS *>(_members);
      serialize<INTROSPECTION_C_MEMBERS>(msg, casted_members);
      break;
    }
    case 1:
    {
      auto casted_members = static_cast<const INTROSPECTION_CPP_MEMBERS *>(_members);
      serialize<INTROSPECTION_CPP_MEMBERS>(msg, casted_members);
      break;
    }
  }
}

template<typename MembersType>
void DesertPublisher::serialize(const void * msg, const MembersType * casted_members)
{
  for (uint32_t i = 0; i < casted_members->member_count_; ++i) {
    const auto member = casted_members->members_ + i;
    void * field = const_cast<char *>(static_cast<const char *>(msg)) + member->offset_;
    switch (member->type_id_) {
      case ::rosidl_typesupport_introspection_cpp::ROS_TYPE_BOOLEAN:
        printf("DATA TYPE BOOL\n");
        break;
      case ::rosidl_typesupport_introspection_cpp::ROS_TYPE_OCTET:
        printf("DATA TYPE OCTET\n");
        break;
      case ::rosidl_typesupport_introspection_cpp::ROS_TYPE_UINT8:
        printf("DATA TYPE UINT8\n");
        break;
      case ::rosidl_typesupport_introspection_cpp::ROS_TYPE_CHAR:
        printf("DATA TYPE CHAR\n");
        break;
      case ::rosidl_typesupport_introspection_cpp::ROS_TYPE_INT8:
        printf("DATA TYPE INT8\n");
        break;
      case ::rosidl_typesupport_introspection_cpp::ROS_TYPE_FLOAT:
        printf("DATA TYPE FLOAT\n");
        break;
      case ::rosidl_typesupport_introspection_cpp::ROS_TYPE_DOUBLE:
        printf("DATA TYPE DOUBLE\n");
        break;
      case ::rosidl_typesupport_introspection_cpp::ROS_TYPE_INT16:
        printf("DATA TYPE INT16\n");
        break;
      case ::rosidl_typesupport_introspection_cpp::ROS_TYPE_UINT16:
        printf("DATA TYPE UINT16\n");
        break;
      case ::rosidl_typesupport_introspection_cpp::ROS_TYPE_INT32:
        printf("DATA TYPE INT32\n");
        break;
      case ::rosidl_typesupport_introspection_cpp::ROS_TYPE_UINT32:
        printf("DATA TYPE UINT32\n");
        break;
      case ::rosidl_typesupport_introspection_cpp::ROS_TYPE_INT64:
        printf("DATA TYPE INT64\n");
        break;
      case ::rosidl_typesupport_introspection_cpp::ROS_TYPE_UINT64:
        printf("DATA TYPE UINT64\n");
        break;
      case ::rosidl_typesupport_introspection_cpp::ROS_TYPE_STRING:
        printf("DATA TYPE STRING\n");
        serialize_field<std::string>(member, field);
        break;
      case ::rosidl_typesupport_introspection_cpp::ROS_TYPE_WSTRING:
        printf("DATA TYPE WSTRING\n");
        break;
      case ::rosidl_typesupport_introspection_cpp::ROS_TYPE_MESSAGE:
        printf("DATA TYPE MESSAGE\n");
        break;
      default:
        throw std::runtime_error("unknown type");
    }
  }
}

// C++ specialization
template<typename T>
void DesertPublisher::serialize_field(const INTROSPECTION_CPP_MEMBER * member, void * field)
{
  if (!member->is_array_) {
    _data_stream << * static_cast<T *>(field);
  }
}

// C specialization
template<typename T>
void DesertPublisher::serialize_field(const INTROSPECTION_C_MEMBER * member, void * field)
{
  if constexpr(std::is_same_v<T, std::string>)
  {
    auto c_string = static_cast<const rosidl_runtime_c__String *>(field);
    _data_stream << std::string(c_string->data);
  }
  else
  {
      /* general code */
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
