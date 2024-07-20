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
}

template<typename MembersType>
void DesertSubscriber::deserialize(void * msg, const MembersType * casted_members)
{
  for (uint32_t i = 0; i < casted_members->member_count_; ++i) {
    const auto member = casted_members->members_ + i;
    void * field = static_cast<char *>(msg) + member->offset_;
    switch (member->type_id_) {
      case ::rosidl_typesupport_introspection_cpp::ROS_TYPE_MESSAGE:
        {
          auto sub_members = static_cast<const MembersType *>(member->members_->data);
          if (!member->is_array_) {
            deserialize(field, sub_members);
          }
          else if (member->array_size_ && !member->is_upper_bound_)
          {
            for (size_t index = 0; index < member->array_size_; ++index) {
              deserialize(member->get_function(field, index), sub_members);
            }
          }
          else
          {
            // Deserialize length
            uint32_t array_size = 0;
            _data_stream >> array_size;
            
            auto vector = reinterpret_cast<std::vector<unsigned char> *>(field);
            new(vector) std::vector<unsigned char>;
            member->resize_function(field, array_size);
            
            for (size_t index = 0; index < array_size; ++index) {
              deserialize(member->get_function(field, index), sub_members);
            }
          }
        }
        break;
      case ::rosidl_typesupport_introspection_cpp::ROS_TYPE_BOOLEAN:
        deserialize_field<bool>(member, field);
        break;
      case ::rosidl_typesupport_introspection_cpp::ROS_TYPE_OCTET:
        //throw std::runtime_error("OCTET type unsupported");
        break;
      case ::rosidl_typesupport_introspection_cpp::ROS_TYPE_UINT8:
        deserialize_field<uint8_t>(member, field);
        break;
      case ::rosidl_typesupport_introspection_cpp::ROS_TYPE_CHAR:
        deserialize_field<char>(member, field);
        break;
      case ::rosidl_typesupport_introspection_cpp::ROS_TYPE_INT8:
        deserialize_field<int8_t>(member, field);
        break;
      case ::rosidl_typesupport_introspection_cpp::ROS_TYPE_FLOAT:
        deserialize_field<float>(member, field);
        break;
      case ::rosidl_typesupport_introspection_cpp::ROS_TYPE_DOUBLE:
        deserialize_field<double>(member, field);
        break;
      case ::rosidl_typesupport_introspection_cpp::ROS_TYPE_INT16:
        deserialize_field<int16_t>(member, field);
        break;
      case ::rosidl_typesupport_introspection_cpp::ROS_TYPE_UINT16:
        deserialize_field<uint16_t>(member, field);
        break;
      case ::rosidl_typesupport_introspection_cpp::ROS_TYPE_INT32:
        deserialize_field<int32_t>(member, field);
        break;
      case ::rosidl_typesupport_introspection_cpp::ROS_TYPE_UINT32:
        deserialize_field<uint32_t>(member, field);
        break;
      case ::rosidl_typesupport_introspection_cpp::ROS_TYPE_INT64:
        deserialize_field<int64_t>(member, field);
        break;
      case ::rosidl_typesupport_introspection_cpp::ROS_TYPE_UINT64:
        deserialize_field<uint64_t>(member, field);
        break;
      case ::rosidl_typesupport_introspection_cpp::ROS_TYPE_STRING:
        deserialize_field<std::string>(member, field);
        break;
      case ::rosidl_typesupport_introspection_cpp::ROS_TYPE_WSTRING:
        deserialize_field<std::u16string>(member, field);
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
    _data_stream >> *static_cast<T *>(field);
  }
  else if (member->array_size_ && !member->is_upper_bound_)
  {
    _data_stream.deserialize_sequence(static_cast<T *>(field), member->array_size_);
  }
  else
  {
    auto & vector = *reinterpret_cast<std::vector<T> *>(field);
    new(&vector) std::vector<T>;
    _data_stream >> vector;
  }
}

// C specialization
template<typename T>
void DesertSubscriber::deserialize_field(const INTROSPECTION_C_MEMBER * member, void * field)
{
  // String specific implementation
  if constexpr(std::is_same_v<T, std::string>)
  {
    if (!member->is_array_) 
    {
      CStringHelper::assign_string(_data_stream, field);
    }
    else if (member->array_size_ && !member->is_upper_bound_)
    {
      std::vector<std::string> cpp_string_vector;
      _data_stream >> cpp_string_vector;
      
      CStringHelper::assign_vector_string(cpp_string_vector, field, member->array_size_);
    }
    else
    {
      printf("WARNING: non-fixed size sequences are currently sperimental\n");
      std::vector<std::string> cpp_string_vector;
      _data_stream >> cpp_string_vector;
      
      CStringHelper::assign_vector_string_to_sequence(cpp_string_vector, field);
    }
  }
  // U16string specific implementation
  else if constexpr(std::is_same_v<T, std::u16string>)
  {
    if (!member->is_array_) 
    {
      CStringHelper::assign_u16string(_data_stream, field);
    }
    else if (member->array_size_ && !member->is_upper_bound_)
    {
      std::vector<std::u16string> cpp_string_vector;
      _data_stream >> cpp_string_vector;
      
      CStringHelper::assign_vector_u16string(cpp_string_vector, field, member->array_size_);
    }
    else
    {
      printf("WARNING: non-fixed size sequences are currently sperimental\n");
      std::vector<std::u16string> cpp_string_vector;
      _data_stream >> cpp_string_vector;
      
      CStringHelper::assign_vector_u16string_to_sequence(cpp_string_vector, field);
    }
  }
  // Generic implementation
  else
  {
    if (!member->is_array_) 
    {
      _data_stream >> * static_cast<T *>(field);
    }
    else if (member->array_size_ && !member->is_upper_bound_)
    {
      _data_stream.deserialize_sequence(static_cast<T *>(field), member->array_size_);
    }
    else
    {
      printf("WARNING: non-fixed size sequences are currently sperimental\n");
      auto & data = *reinterpret_cast<typename GenericCSequence<T>::type *>(field);
      uint32_t size = 0;
      _data_stream >> size;
      size_t dsize = static_cast<size_t>(size);
      
      if (!GenericCSequence<T>::init(&data, dsize))
      {
        throw std::runtime_error("unable to initialize GenericCSequence");
      }
      
      _data_stream.deserialize_sequence(reinterpret_cast<T *>(data.data), dsize);
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
