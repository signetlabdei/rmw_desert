#include "DesertClient.h"

DesertClient::DesertClient(std::string service_name, const rosidl_service_type_support_t * type_supports, rmw_gid_t gid)
      : _name(service_name)
      , _id(TopicsConfig::get_topic_identifier(service_name))
      , _request_data_stream(cbor::TxStream(CLIENT_TYPE, service_name, _id))
      , _response_data_stream(cbor::RxStream(CLIENT_TYPE, service_name, _id))
      , _gid(gid)
{
  const rosidl_service_type_support_t * service_type_support = get_service_type_support(type_supports);
  _service = get_service(service_type_support);
}

bool DesertClient::has_data()
{
  cbor::RxStream::interpret_packets();
  return _response_data_stream.data_available(_sequence_id);
}

void DesertClient::read_response(void * res, rmw_service_info_t * req_header)
{
  req_header->request_id.sequence_number = _sequence_id;
  
  if (_c_cpp_identifier == 0)
  {
    auto casted_service = static_cast<const INTROSPECTION_C_SERVICE_MEMBERS *>(_service);
    MessageSerialization::deserialize(res, casted_service->response_members_, _response_data_stream);
  }
  else if (_c_cpp_identifier == 1)
  {
    auto casted_service = static_cast<const INTROSPECTION_CPP_SERVICE_MEMBERS *>(_service);
    MessageSerialization::deserialize(res, casted_service->response_members_, _response_data_stream);
  }
  
  _response_data_stream.clear_buffer();
}

void DesertClient::send_request(const void * req, int64_t * sequence_id)
{
  // Stream identifier equals to zero means that the corresponding service is disabled
  if (_id == 0) return;
  
  _sequence_id = *sequence_id;
  
  _request_data_stream.start_transmission(_sequence_id);
  
  if (_c_cpp_identifier == 0)
  {
    auto casted_service = static_cast<const INTROSPECTION_C_SERVICE_MEMBERS *>(_service);
    MessageSerialization::serialize(req, casted_service->request_members_, _request_data_stream);
  }
  else if (_c_cpp_identifier == 1)
  {
    auto casted_service = static_cast<const INTROSPECTION_CPP_SERVICE_MEMBERS *>(_service);
    MessageSerialization::serialize(req, casted_service->request_members_, _request_data_stream);
  }
  
  _request_data_stream.end_transmission();
}

rmw_gid_t DesertClient::get_gid()
{
  return _gid;
}

std::string DesertClient::get_service_name()
{
  return _name;
}

std::string DesertClient::get_request_type_name()
{
  std::string namespace_;
  std::string name_;
    
  if (_c_cpp_identifier == 0)
  {
    auto casted_service = static_cast<const INTROSPECTION_C_SERVICE_MEMBERS *>(_service);
    namespace_ = casted_service->request_members_->message_namespace_;
    name_ = casted_service->request_members_->message_name_;
  }
  else if (_c_cpp_identifier == 1)
  {
    auto casted_service = static_cast<const INTROSPECTION_CPP_SERVICE_MEMBERS *>(_service);
    namespace_ = casted_service->request_members_->message_namespace_;
    name_ = casted_service->request_members_->message_name_;
  }

  std::string namespace_sleshed = regex_replace(namespace_, std::regex("::"), "/");
  
  return namespace_sleshed + "/" + name_;
}

std::string DesertClient::get_response_type_name()
{
  std::string namespace_;
  std::string name_;
    
  if (_c_cpp_identifier == 0)
  {
    auto casted_service = static_cast<const INTROSPECTION_C_SERVICE_MEMBERS *>(_service);
    namespace_ = casted_service->response_members_->message_namespace_;
    name_ = casted_service->response_members_->message_name_;
  }
  else if (_c_cpp_identifier == 1)
  {
    auto casted_service = static_cast<const INTROSPECTION_CPP_SERVICE_MEMBERS *>(_service);
    namespace_ = casted_service->response_members_->message_namespace_;
    name_ = casted_service->response_members_->message_name_;
  }

  std::string namespace_sleshed = regex_replace(namespace_, std::regex("::"), "/");
  
  return namespace_sleshed + "/" + name_;
}

const void * DesertClient::get_service(const rosidl_service_type_support_t * service_type_support)
{
  return service_type_support->data;
}

const rosidl_service_type_support_t * DesertClient::get_service_type_support(const rosidl_service_type_support_t * type_supports)
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
