#include "rmw/rmw.h"


rmw_ret_t rmw_borrow_loaned_message(const rmw_publisher_t * publisher, const rosidl_message_type_support_t * type_support, void ** ros_message)
{
  return RMW_RET_OK;
}

rmw_ret_t rmw_client_request_publisher_get_actual_qos(const rmw_client_t * client, rmw_qos_profile_t * qos)
{
  return RMW_RET_OK;
}

rmw_ret_t rmw_client_response_subscription_get_actual_qos(const rmw_client_t * client, rmw_qos_profile_t * qos)
{
  return RMW_RET_OK;
}

rmw_ret_t rmw_client_set_on_new_response_callback(rmw_client_t * client, rmw_event_callback_t callback, const void * user_data)
{
  return RMW_RET_OK;
}

rmw_ret_t rmw_compare_gids_equal(const rmw_gid_t * gid1,  const rmw_gid_t * gid2,  bool * result)
{
  return RMW_RET_OK;
}

rmw_ret_t rmw_count_clients(const rmw_node_t * node, const char * service_name, size_t * count)
{
  return RMW_RET_OK;
}

rmw_ret_t rmw_count_publishers(const rmw_node_t * node, const char * topic_name, size_t * count)
{
  return RMW_RET_OK;
}

rmw_ret_t rmw_count_services(const rmw_node_t * node, const char * service_name, size_t * count)
{
  return RMW_RET_OK;
}

rmw_ret_t rmw_count_subscribers(const rmw_node_t * node, const char * topic_name, size_t * count)
{
  return RMW_RET_OK;
}

rmw_client_t * rmw_create_client(const rmw_node_t * node, const rosidl_service_type_support_t * type_support, const char * service_name, const rmw_qos_profile_t * qos_policies)
{
  return nullptr;
}

rmw_guard_condition_t * rmw_create_guard_condition(rmw_context_t * context)
{
  return nullptr;
}

rmw_node_t * rmw_create_node(rmw_context_t * context, const char * name, const char * namespace_)
{
  return nullptr;
}

rmw_publisher_t * rmw_create_publisher(const rmw_node_t * node, const rosidl_message_type_support_t * type_support, const char * topic_name, const rmw_qos_profile_t * qos_profile, const rmw_publisher_options_t * publisher_options)
{
  return nullptr;
}

rmw_service_t * rmw_create_service(const rmw_node_t * node, const rosidl_service_type_support_t * type_support, const char * service_name, const rmw_qos_profile_t * qos_profile)
{
  return nullptr;
}

rmw_subscription_t * rmw_create_subscription(const rmw_node_t * node, const rosidl_message_type_support_t * type_support, const char * topic_name, const rmw_qos_profile_t * qos_policies, const rmw_subscription_options_t * subscription_options)
{
  return nullptr;
}

rmw_wait_set_t * rmw_create_wait_set(rmw_context_t * context,  size_t max_conditions)
{
  return nullptr;
}

rmw_ret_t rmw_deserialize(const rmw_serialized_message_t * serialized_message, const rosidl_message_type_support_t * type_support, void * ros_message)
{
  return RMW_RET_OK;
}

rmw_ret_t rmw_destroy_client(rmw_node_t * node,  rmw_client_t * client)
{
  return RMW_RET_OK;
}

rmw_ret_t rmw_destroy_guard_condition(rmw_guard_condition_t * guard_condition)
{
  return RMW_RET_OK;
}

rmw_ret_t rmw_destroy_node(rmw_node_t * node)
{
  return RMW_RET_OK;
}

rmw_ret_t rmw_destroy_publisher(rmw_node_t * node,  rmw_publisher_t * publisher)
{
  return RMW_RET_OK;
}

rmw_ret_t rmw_destroy_service(rmw_node_t * node,  rmw_service_t * service)
{
  return RMW_RET_OK;
}

rmw_ret_t rmw_destroy_subscription(rmw_node_t * node,  rmw_subscription_t * subscription)
{
  return RMW_RET_OK;
}

rmw_ret_t rmw_destroy_wait_set(rmw_wait_set_t * wait_set)
{
  return RMW_RET_OK;
}

rmw_ret_t rmw_event_set_callback(rmw_event_t * event, rmw_event_callback_t callback, const void * user_data)
{
  return RMW_RET_OK;
}

rmw_ret_t rmw_fini_publisher_allocation(rmw_publisher_allocation_t * allocation)
{
  return RMW_RET_OK;
}

rmw_ret_t rmw_fini_subscription_allocation(rmw_subscription_allocation_t * allocation)
{
  return RMW_RET_OK;
}

rmw_ret_t rmw_get_gid_for_client(const rmw_client_t * client,  rmw_gid_t * gid)
{
  return RMW_RET_OK;
}

rmw_ret_t rmw_get_gid_for_publisher(const rmw_publisher_t * publisher,  rmw_gid_t * gid)
{
  return RMW_RET_OK;
}

const char * rmw_get_implementation_identifier(void)
{
  static const char * fake_impl_id = "rmw_desert";
  return fake_impl_id;
}

rmw_ret_t rmw_get_node_names(const rmw_node_t * node, rcutils_string_array_t * node_names, rcutils_string_array_t * node_namespaces)
{
  return RMW_RET_OK;
}

rmw_ret_t rmw_get_node_names_with_enclaves(const rmw_node_t * node, rcutils_string_array_t * node_names, rcutils_string_array_t * node_namespaces, rcutils_string_array_t * enclaves)
{
  return RMW_RET_OK;
}

const char * rmw_get_serialization_format(void)
{
  return "";
}

rmw_ret_t rmw_get_serialized_message_size(const rosidl_message_type_support_t * type_support, const rosidl_runtime_c__Sequence__bound * message_bounds, size_t * size)
{
  return RMW_RET_OK;
}

rmw_ret_t rmw_init_publisher_allocation(const rosidl_message_type_support_t * type_support, const rosidl_runtime_c__Sequence__bound * message_bounds, rmw_publisher_allocation_t * allocation)
{
  return RMW_RET_OK;
}

rmw_ret_t rmw_init_subscription_allocation(const rosidl_message_type_support_t * type_support, const rosidl_runtime_c__Sequence__bound * message_bounds, rmw_subscription_allocation_t * allocation)
{
  return RMW_RET_OK;
}

rmw_ret_t rmw_node_assert_liveliness(const rmw_node_t * node)
{
  return RMW_RET_OK;
}

const rmw_guard_condition_t * rmw_node_get_graph_guard_condition(const rmw_node_t * node)
{
  return nullptr;
}

rmw_ret_t rmw_publish(const rmw_publisher_t * publisher, const void * ros_message, rmw_publisher_allocation_t * allocation)
{
  return RMW_RET_OK;
}

rmw_ret_t rmw_publish_loaned_message(const rmw_publisher_t * publisher, void * ros_message, rmw_publisher_allocation_t * allocation)
{
  return RMW_RET_OK;
}

rmw_ret_t rmw_publish_serialized_message(const rmw_publisher_t * publisher, const rmw_serialized_message_t * serialized_message, rmw_publisher_allocation_t * allocation)
{
  return RMW_RET_OK;
}

rmw_ret_t rmw_publisher_assert_liveliness(const rmw_publisher_t * publisher)
{
  return RMW_RET_OK;
}

rmw_ret_t rmw_publisher_count_matched_subscriptions(const rmw_publisher_t * publisher, size_t * subscription_count)
{
  return RMW_RET_OK;
}

rmw_ret_t rmw_publisher_get_actual_qos(const rmw_publisher_t * publisher, rmw_qos_profile_t * qos)
{
  return RMW_RET_OK;
}

rmw_ret_t rmw_publisher_wait_for_all_acked(const rmw_publisher_t * publisher, rmw_time_t wait_timeout)
{
  return RMW_RET_OK;
}

rmw_ret_t rmw_return_loaned_message_from_publisher(const rmw_publisher_t * publisher, void * loaned_message)
{
  return RMW_RET_OK;
}

rmw_ret_t rmw_return_loaned_message_from_subscription(const rmw_subscription_t * subscription, void * loaned_message)
{
  return RMW_RET_OK;
}

rmw_ret_t rmw_send_request(const rmw_client_t * client, const void * ros_request, int64_t * sequence_id)
{
  return RMW_RET_OK;
}

rmw_ret_t rmw_send_response(const rmw_service_t * service, rmw_request_id_t * request_header, void * ros_response)
{
  return RMW_RET_OK;
}

rmw_ret_t rmw_serialize(const void * ros_message, const rosidl_message_type_support_t * type_support, rmw_serialized_message_t * serialized_message)
{
  return RMW_RET_OK;
}

rmw_ret_t rmw_service_request_subscription_get_actual_qos(const rmw_service_t * service, rmw_qos_profile_t * qos)
{
  return RMW_RET_OK;
}

rmw_ret_t rmw_service_response_publisher_get_actual_qos(const rmw_service_t * service, rmw_qos_profile_t * qos)
{
  return RMW_RET_OK;
}

rmw_ret_t rmw_service_server_is_available(const rmw_node_t * node, const rmw_client_t * client, bool * is_available)
{
  return RMW_RET_OK;
}

rmw_ret_t rmw_service_set_on_new_request_callback(rmw_service_t * service, rmw_event_callback_t callback, const void * user_data)
{
  return RMW_RET_OK;
}

rmw_ret_t rmw_set_log_severity(rmw_log_severity_t severity)
{
  return RMW_RET_OK;
}

rmw_ret_t rmw_subscription_count_matched_publishers(const rmw_subscription_t * subscription, size_t * publisher_count)
{
  return RMW_RET_OK;
}

rmw_ret_t rmw_subscription_get_actual_qos(const rmw_subscription_t * subscription, rmw_qos_profile_t * qos)
{
  return RMW_RET_OK;
}

rmw_ret_t rmw_subscription_get_content_filter(const rmw_subscription_t * subscription, rcutils_allocator_t * allocator, rmw_subscription_content_filter_options_t * options)
{
  return RMW_RET_OK;
}

rmw_ret_t rmw_subscription_set_content_filter(rmw_subscription_t * subscription, const rmw_subscription_content_filter_options_t * options)
{
  return RMW_RET_OK;
}

rmw_ret_t rmw_subscription_set_on_new_message_callback(rmw_subscription_t * subscription, rmw_event_callback_t callback, const void * user_data)
{
  return RMW_RET_OK;
}

rmw_ret_t rmw_take(const rmw_subscription_t * subscription, void * ros_message, bool * taken, rmw_subscription_allocation_t * allocation)
{
  return RMW_RET_OK;
}

rmw_ret_t rmw_take_loaned_message(const rmw_subscription_t * subscription, void ** loaned_message, bool * taken, rmw_subscription_allocation_t * allocation)
{
  return RMW_RET_OK;
}

rmw_ret_t rmw_take_loaned_message_with_info(const rmw_subscription_t * subscription, void ** loaned_message, bool * taken, rmw_message_info_t * message_info, rmw_subscription_allocation_t * allocation)
{
  return RMW_RET_OK;
}

rmw_ret_t rmw_take_request(const rmw_service_t * service, rmw_service_info_t * request_header, void * ros_request, bool * taken)
{
  return RMW_RET_OK;
}

rmw_ret_t rmw_take_response(const rmw_client_t * client, rmw_service_info_t * request_header, void * ros_response, bool * taken)
{
  return RMW_RET_OK;
}

rmw_ret_t rmw_take_sequence(const rmw_subscription_t * subscription, size_t count, rmw_message_sequence_t * message_sequence, rmw_message_info_sequence_t * message_info_sequence, size_t * taken, rmw_subscription_allocation_t * allocation)
{
  return RMW_RET_OK;
}

rmw_ret_t rmw_take_serialized_message(const rmw_subscription_t * subscription, rmw_serialized_message_t * serialized_message, bool * taken, rmw_subscription_allocation_t * allocation)
{
  return RMW_RET_OK;
}

rmw_ret_t rmw_take_serialized_message_with_info(const rmw_subscription_t * subscription, rmw_serialized_message_t * serialized_message, bool * taken, rmw_message_info_t * message_info, rmw_subscription_allocation_t * allocation)
{
  return RMW_RET_OK;
}

rmw_ret_t rmw_take_with_info(const rmw_subscription_t * subscription, void * ros_message, bool * taken, rmw_message_info_t * message_info, rmw_subscription_allocation_t * allocation)
{
  return RMW_RET_OK;
}

rmw_ret_t rmw_trigger_guard_condition(const rmw_guard_condition_t * guard_condition)
{
  return RMW_RET_OK;
}

rmw_ret_t rmw_wait(rmw_subscriptions_t * subscriptions, rmw_guard_conditions_t * guard_conditions, rmw_services_t * services, rmw_clients_t * clients, rmw_events_t * events, rmw_wait_set_t * wait_set, const rmw_time_t * wait_timeout)
{
  return RMW_RET_OK;
}

