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

void * DesertSubscriber::read_data()
{
  /*std::vector<std::pair<void *, int>> packet;
  _data_stream >> packet;
  
  if (packet.size() > 0)
  {
    for (int i=0; i < packet.size(); i++)
    {
      if (packet[i].second == CBOR_ITEM_STRING)
      {
        printf("FIELD %i: %s\n", i, static_cast<std::string *>(packet[i].first)->c_str());
      }
      else if (packet[0].second == CBOR_ITEM_INTEGER)
      {
        printf("FIELD %i: %i\n", i, *static_cast<uint32_t *>(packet[i].first));
      }
    }
  }*/
  
  char aa[4096];
  void * msg = (void*)aa;
  switch (_c_cpp_identifier)
  {
    case 0:
    {
      auto casted_members = static_cast<const INTROSPECTION_C_MEMBERS *>(_members);
      deserialize<INTROSPECTION_C_MEMBERS>(msg, casted_members);
      break;
    }
    case 1:
    {
      auto casted_members = static_cast<const INTROSPECTION_CPP_MEMBERS *>(_members);
      deserialize<INTROSPECTION_CPP_MEMBERS>(msg, casted_members);
      break;
    }
  }
  
  return nullptr;
}

template<typename MembersType>
void DesertSubscriber::deserialize(void * msg, const MembersType * casted_members)
{
  for (uint32_t i = 0; i < casted_members->member_count_; ++i) {
    const auto member = casted_members->members_ + i;
    void * field = static_cast<char *>(msg) + member->offset_;
    switch (member->type_id_) {
      case ::rosidl_typesupport_introspection_cpp::ROS_TYPE_MESSAGE:
        break;
      case ::rosidl_typesupport_introspection_cpp::ROS_TYPE_BOOLEAN:
        break;
      case ::rosidl_typesupport_introspection_cpp::ROS_TYPE_OCTET:
        throw std::runtime_error("OCTET type unsupported");
        break;
      case ::rosidl_typesupport_introspection_cpp::ROS_TYPE_UINT8:
        break;
      case ::rosidl_typesupport_introspection_cpp::ROS_TYPE_CHAR:
        break;
      case ::rosidl_typesupport_introspection_cpp::ROS_TYPE_INT8:
        break;
      case ::rosidl_typesupport_introspection_cpp::ROS_TYPE_FLOAT:
        break;
      case ::rosidl_typesupport_introspection_cpp::ROS_TYPE_DOUBLE:
        break;
      case ::rosidl_typesupport_introspection_cpp::ROS_TYPE_INT16:
        break;
      case ::rosidl_typesupport_introspection_cpp::ROS_TYPE_UINT16:
        break;
      case ::rosidl_typesupport_introspection_cpp::ROS_TYPE_INT32:
        break;
      case ::rosidl_typesupport_introspection_cpp::ROS_TYPE_UINT32:
        break;
      case ::rosidl_typesupport_introspection_cpp::ROS_TYPE_INT64:
        break;
      case ::rosidl_typesupport_introspection_cpp::ROS_TYPE_UINT64:
        break;
      case ::rosidl_typesupport_introspection_cpp::ROS_TYPE_STRING:
        deserialize_field<std::string>(member, field);
        break;
      case ::rosidl_typesupport_introspection_cpp::ROS_TYPE_WSTRING:
        break;
      default:
        throw std::runtime_error("unknown type");
    }
  }
}

// C++ specialization
template<typename T>
void DesertSubscriber::deserialize_field(const INTROSPECTION_CPP_MEMBER * member, void * field)
{
  if (!member->is_array_) {
    std::string dato;
    _data_stream >> dato;
    if (!dato.empty())
      printf("FIELD: %s\n", dato.c_str());
  }
}

// C specialization
template<typename T>
void DesertSubscriber::deserialize_field(const INTROSPECTION_C_MEMBER * member, void * field)
{
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
