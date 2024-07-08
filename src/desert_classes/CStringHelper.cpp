#include "CStringHelper.h"

std::string CStringHelper::convert_to_std_string(void * str)
{
  auto c_string = static_cast<const rosidl_runtime_c__String *>(str);
  return std::string(c_string->data);
}

std::vector<std::string> CStringHelper::convert_to_std_vector_string(void * str_array, size_t size)
{
  std::vector<std::string> cpp_string_vector;
  auto string_field = static_cast<rosidl_runtime_c__String *>(str_array);
  for (size_t i = 0; i < size; ++i)
  {
    auto c_string = static_cast<const rosidl_runtime_c__String>(string_field[i]);
    cpp_string_vector.push_back(std::string(c_string.data));
  }
  return cpp_string_vector;
}

std::vector<std::string> CStringHelper::convert_sequence_to_std_vector_string(void * str_seq)
{
  std::vector<std::string> cpp_string_vector;
  auto & string_sequence_field = *reinterpret_cast<rosidl_runtime_c__String__Sequence *>(str_seq);
  for (size_t i = 0; i < string_sequence_field.size; ++i)
  {
    auto c_string = static_cast<const rosidl_runtime_c__String>(string_sequence_field.data[i]);
    cpp_string_vector.push_back(std::string(c_string.data));
  }
  return cpp_string_vector;
}

std::u16string CStringHelper::convert_to_std_u16string(void * str)
{
  auto c_u16string = static_cast<const rosidl_runtime_c__U16String *>(str);
  return std::u16string(reinterpret_cast<char16_t *>(c_u16string->data));
}

std::vector<std::u16string> CStringHelper::convert_to_std_vector_u16string(void * str_array, size_t size)
{
  std::vector<std::u16string> cpp_string_vector;
  auto u16string_field = static_cast<rosidl_runtime_c__U16String *>(str_array);
  for (size_t i = 0; i < size; ++i)
  {
    auto c_u16string = static_cast<const rosidl_runtime_c__U16String>(u16string_field[i]);
    cpp_string_vector.push_back(reinterpret_cast<char16_t *>(c_u16string.data));
  }
  return cpp_string_vector;
}

std::vector<std::u16string> CStringHelper::convert_sequence_to_std_vector_u16string(void * str_seq)
{
  std::vector<std::u16string> cpp_string_vector;
  auto & u16string_sequence_field = *reinterpret_cast<rosidl_runtime_c__U16String__Sequence *>(str_seq);
  for (size_t i = 0; i < u16string_sequence_field.size; ++i)
  {
    auto c_u16string = static_cast<const rosidl_runtime_c__U16String>(u16string_sequence_field.data[i]);
    cpp_string_vector.push_back(reinterpret_cast<char16_t *>(c_u16string.data));
  }
  return cpp_string_vector;
}

