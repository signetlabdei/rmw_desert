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

void CStringHelper::assign_string(std::string str, void * field)
{
  rosidl_runtime_c__String * c_str = static_cast<rosidl_runtime_c__String *>(field);
  rosidl_runtime_c__String__assign(c_str, str.c_str());
}

void CStringHelper::assign_vector_string(std::vector<std::string> cpp_string_vector, void * str_array, size_t size)
{
  auto string_field = static_cast<rosidl_runtime_c__String *>(str_array);
  for (size_t i = 0; i < size; ++i)
  {
    if (!rosidl_runtime_c__String__assign(&string_field[i], cpp_string_vector[i].c_str()))
    {
      throw std::runtime_error("unable to assign rosidl_runtime_c__String");
    }
  }
}

void CStringHelper::assign_vector_string_to_sequence(std::vector<std::string> cpp_string_vector, void * str_seq)
{
  auto & string_sequence_field = *reinterpret_cast<rosidl_runtime_c__String__Sequence *>(str_seq);
  
  if (!rosidl_runtime_c__String__Sequence__init(&string_sequence_field, cpp_string_vector.size()))
  {
    throw std::runtime_error("unable to initialize rosidl_generator_c__String sequence");
  }
  
  for (size_t i = 0; i < cpp_string_vector.size(); ++i)
  {
    if (!rosidl_runtime_c__String__assign(&string_sequence_field.data[i], cpp_string_vector[i].c_str()))
    {
      throw std::runtime_error("unable to assign rosidl_runtime_c__String");
    }
  }
}

void CStringHelper::assign_u16string(std::u16string str, void * field)
{
  rosidl_runtime_c__U16String * c_str = static_cast<rosidl_runtime_c__U16String *>(field);
  rosidl_runtime_c__U16String__assign(c_str, reinterpret_cast<const uint16_t *>(str.c_str()));
}

void CStringHelper::assign_vector_u16string(std::vector<std::u16string> cpp_string_vector, void * str_array, size_t size)
{
  auto string_field = static_cast<rosidl_runtime_c__U16String *>(str_array);
  for (size_t i = 0; i < size; ++i)
  {
    if (!rosidl_runtime_c__U16String__assign(&string_field[i], reinterpret_cast<const uint16_t *>(cpp_string_vector[i].c_str())))
    {
      throw std::runtime_error("unable to assign rosidl_runtime_c__U16String");
    }
  }
}

void CStringHelper::assign_vector_u16string_to_sequence(std::vector<std::u16string> cpp_string_vector, void * str_seq)
{
  auto & string_sequence_field = *reinterpret_cast<rosidl_runtime_c__U16String__Sequence *>(str_seq);
  
  if (!rosidl_runtime_c__U16String__Sequence__init(&string_sequence_field, cpp_string_vector.size()))
  {
    throw std::runtime_error("unable to initialize rosidl_generator_c__U16String sequence");
  }
  
  for (size_t i = 0; i < cpp_string_vector.size(); ++i)
  {
    if (!rosidl_runtime_c__U16String__assign(&string_sequence_field.data[i], reinterpret_cast<const uint16_t *>(cpp_string_vector[i].c_str())))
    {
      throw std::runtime_error("unable to assign rosidl_runtime_c__U16String");
    }
  }
}
