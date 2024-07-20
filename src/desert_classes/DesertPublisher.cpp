#include "DesertPublisher.h"

DesertPublisher::DesertPublisher(std::string topic_name, uint64_t id, const rosidl_message_type_support_t * type_supports)
      : _name(topic_name)
      , _id(id)
      , _data_stream(cbor::TxStream())
{
  const rosidl_message_type_support_t * type_support = get_type_support(type_supports);
  _members = get_members(type_support);
}

void DesertPublisher::push(const void * msg)
{
  if (_name == "/rosout") return;
  
  _data_stream.start_transmission(_name);
  
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
  
  _data_stream.end_transmission();
}

template<typename MembersType>
void DesertPublisher::serialize(const void * msg, const MembersType * casted_members)
{
  for (uint32_t i = 0; i < casted_members->member_count_; ++i) {
    const auto member = casted_members->members_ + i;
    void * field = const_cast<char *>(static_cast<const char *>(msg)) + member->offset_;
    switch (member->type_id_) {
      case ::rosidl_typesupport_introspection_cpp::ROS_TYPE_MESSAGE:
        {
          auto sub_members = static_cast<const MembersType *>(member->members_->data);
          if (!member->is_array_) {
            serialize(field, sub_members);
          }
          else if (member->array_size_ && !member->is_upper_bound_)
          {
            for (size_t index = 0; index < member->array_size_; ++index) {
              serialize(member->get_function(field, index), sub_members);
            }
          }
          else
          {
            size_t array_size = member->size_function(field);
            
            if (member->is_upper_bound_ && array_size > member->array_size_)
            {
              throw std::runtime_error("Sequence overcomes the maximum length");
            }
            
            // Serialize length
            _data_stream << (uint32_t)array_size;
            
            for (size_t index = 0; index < array_size; ++index) {
              serialize(member->get_function(field, index), sub_members);
            }
          }
        }
        break;
      case ::rosidl_typesupport_introspection_cpp::ROS_TYPE_BOOLEAN:
        if (!member->is_array_)
        {
          // Don't cast to bool here because if the bool is uninitialized the random value can't be deserialized
          _data_stream << (*static_cast<uint8_t *>(field) ? true : false);
        }
        else
        {
          serialize_field<bool>(member, field);
        }
        break;
      case ::rosidl_typesupport_introspection_cpp::ROS_TYPE_OCTET:
        //throw std::runtime_error("OCTET type unsupported");
        break;
      case ::rosidl_typesupport_introspection_cpp::ROS_TYPE_UINT8:
        serialize_field<uint8_t>(member, field);
        break;
      case ::rosidl_typesupport_introspection_cpp::ROS_TYPE_CHAR:
        serialize_field<char>(member, field);
        break;
      case ::rosidl_typesupport_introspection_cpp::ROS_TYPE_INT8:
        serialize_field<int8_t>(member, field);
        break;
      case ::rosidl_typesupport_introspection_cpp::ROS_TYPE_FLOAT:
        serialize_field<float>(member, field);
        break;
      case ::rosidl_typesupport_introspection_cpp::ROS_TYPE_DOUBLE:
        serialize_field<double>(member, field);
        break;
      case ::rosidl_typesupport_introspection_cpp::ROS_TYPE_INT16:
        serialize_field<int16_t>(member, field);
        break;
      case ::rosidl_typesupport_introspection_cpp::ROS_TYPE_UINT16:
        serialize_field<uint16_t>(member, field);
        break;
      case ::rosidl_typesupport_introspection_cpp::ROS_TYPE_INT32:
        serialize_field<int32_t>(member, field);
        break;
      case ::rosidl_typesupport_introspection_cpp::ROS_TYPE_UINT32:
        serialize_field<uint32_t>(member, field);
        break;
      case ::rosidl_typesupport_introspection_cpp::ROS_TYPE_INT64:
        serialize_field<int64_t>(member, field);
        break;
      case ::rosidl_typesupport_introspection_cpp::ROS_TYPE_UINT64:
        serialize_field<uint64_t>(member, field);
        break;
      case ::rosidl_typesupport_introspection_cpp::ROS_TYPE_STRING:
        serialize_field<std::string>(member, field);
        break;
      case ::rosidl_typesupport_introspection_cpp::ROS_TYPE_WSTRING:
        serialize_field<std::u16string>(member, field);
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
  if (!member->is_array_)
  {
    _data_stream << * static_cast<T *>(field);
  }
  else if (member->array_size_ && !member->is_upper_bound_)
  {
    _data_stream.serialize_sequence(static_cast<T *>(field), member->array_size_);
  }
  else
  {
    std::vector<T> & data = *reinterpret_cast<std::vector<T> *>(field);
    _data_stream << data;
  }
}

// C specialization
template<typename T>
void DesertPublisher::serialize_field(const INTROSPECTION_C_MEMBER * member, void * field)
{
  // String specific implementation
  if constexpr(std::is_same_v<T, std::string>)
  {
    if (!member->is_array_) 
    {
      _data_stream << CStringHelper::convert_to_std_string(field);
    }
    else if (member->array_size_ && !member->is_upper_bound_)
    {
      _data_stream << CStringHelper::convert_to_std_vector_string(field, member->array_size_);
    }
    else
    {
      printf("WARNING: non-fixed size sequences are currently sperimental\n");
      _data_stream << CStringHelper::convert_sequence_to_std_vector_string(field);
    }
  }
  // U16string specific implementation
  else if constexpr(std::is_same_v<T, std::u16string>)
  {
    if (!member->is_array_) 
    {
      _data_stream << CStringHelper::convert_to_std_u16string(field);
    }
    else if (member->array_size_ && !member->is_upper_bound_)
    {
      _data_stream << CStringHelper::convert_to_std_vector_u16string(field, member->array_size_);
    }
    else
    {
      printf("WARNING: non-fixed size sequences are currently sperimental\n");
      _data_stream << CStringHelper::convert_sequence_to_std_vector_u16string(field);
    }
  }
  // Generic implementation
  else
  {
    if (!member->is_array_) 
    {
      _data_stream << * static_cast<T *>(field);
    }
    else if (member->array_size_ && !member->is_upper_bound_)
    {
      _data_stream.serialize_sequence(static_cast<T *>(field), member->array_size_);
    }
    else
    {
      printf("WARNING: non-fixed size sequences are currently sperimental\n");
      auto & data = *reinterpret_cast<typename GenericCSequence<T>::type *>(field);
      
      // Serialize length
      _data_stream << (uint32_t)data.size;
            
      _data_stream.serialize_sequence(reinterpret_cast<T *>(data.data), data.size);
    }
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
