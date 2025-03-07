#include <stdlib.h>
#include <string.h>

#include "rmw/rmw.h"
#include "rmw/allocators.h"

#include "rosidl_typesupport_introspection_cpp/identifier.hpp"
#include "rosidl_typesupport_introspection_c/identifier.h"
#include "rosidl_typesupport_introspection_cpp/message_introspection.hpp"
#include "rosidl_typesupport_introspection_c/message_introspection.h"
#include "rosidl_typesupport_introspection_cpp/field_types.hpp"
#include "rosidl_typesupport_introspection_c/field_types.h"

#include "classes.h"
#include "debug.h"


rmw_gid_t generate_gid()
{
  rmw_gid_t gid;
  
  gid.implementation_identifier = rmw_get_implementation_identifier();
  
  for (unsigned int i = 0; i < RMW_GID_STORAGE_SIZE; i++)
  {
    gid.data[i] = rand();
  }
  
  return gid;
}

rmw_ret_t rmw_borrow_loaned_message(const rmw_publisher_t * publisher, const rosidl_message_type_support_t * type_support, void ** ros_message)
{
  DEBUG("rmw_borrow_loaned_message" "\n");
  (void) publisher;
  (void) type_support;
  (void) ros_message;
  return RMW_RET_OK;
}

rmw_ret_t rmw_client_request_publisher_get_actual_qos(const rmw_client_t * client, rmw_qos_profile_t * qos)
{
  DEBUG("rmw_client_request_publisher_get_actual_qos" "\n");
  (void) client;
  (void) qos;
  return RMW_RET_OK;
}

rmw_ret_t rmw_client_response_subscription_get_actual_qos(const rmw_client_t * client, rmw_qos_profile_t * qos)
{
  DEBUG("rmw_client_response_subscription_get_actual_qos" "\n");
  (void) client;
  (void) qos;
  return RMW_RET_OK;
}

rmw_ret_t rmw_client_set_on_new_response_callback(rmw_client_t * client, rmw_event_callback_t callback, const void * user_data)
{
  DEBUG("rmw_client_set_on_new_response_callback" "\n");
  (void) client;
  (void) callback;
  (void) user_data;
  return RMW_RET_OK;
}

rmw_ret_t rmw_compare_gids_equal(const rmw_gid_t * gid1,  const rmw_gid_t * gid2,  bool * result)
{
  DEBUG("rmw_compare_gids_equal" "\n");
  (void) gid1;
  (void) gid2;
  (void) result;
  return RMW_RET_OK;
}

rmw_ret_t rmw_count_clients(const rmw_node_t * node, const char * service_name, size_t * count)
{
  DEBUG("rmw_count_clients" "\n");
  
  auto common_context = &node->context->impl->common;
  const std::string mangled_service_name = std::string(Discovery::ros_service_requester_prefix) + std::string(service_name);
  
  return common_context->graph_cache.get_writer_count(mangled_service_name, count);
}

rmw_ret_t rmw_count_publishers(const rmw_node_t * node, const char * topic_name, size_t * count)
{
  DEBUG("rmw_count_publishers" "\n");
  
  auto common_context = &node->context->impl->common;
  const std::string mangled_topic_name = std::string(Discovery::ros_topic_publisher_prefix) + std::string(topic_name);
  
  return common_context->graph_cache.get_writer_count(mangled_topic_name, count);
}

rmw_ret_t rmw_count_services(const rmw_node_t * node, const char * service_name, size_t * count)
{
  DEBUG("rmw_count_services" "\n");
  
  auto common_context = &node->context->impl->common;
  const std::string mangled_service_name = std::string(Discovery::ros_service_response_prefix) + std::string(service_name);
  
  return common_context->graph_cache.get_writer_count(mangled_service_name, count);
}

rmw_ret_t rmw_count_subscribers(const rmw_node_t * node, const char * topic_name, size_t * count)
{
  DEBUG("rmw_count_subscribers" "\n");
  
  auto common_context = &node->context->impl->common;
  const std::string mangled_topic_name = std::string(Discovery::ros_topic_subscriber_prefix) + std::string(topic_name);
  
  return common_context->graph_cache.get_reader_count(mangled_topic_name, count);
}

rmw_client_t * rmw_create_client(const rmw_node_t * node, const rosidl_service_type_support_t * type_supports, const char * service_name, const rmw_qos_profile_t * qos_policies)
{
  DEBUG("rmw_create_client" "\n");
  (void) qos_policies;
  
  if (strcmp(service_name, "/discovery") == 0 || strcmp(service_name, "/discovery_request") == 0)
  {
    RMW_SET_ERROR_MSG("Client applications are not allowed to use '/discovery' and '/discovery_request' topic names");
    return nullptr;
  }

  rmw_client_t * ret = rmw_client_allocate();
  ret->implementation_identifier = rmw_get_implementation_identifier();
  ret->service_name = service_name;
  
  DesertClient * cli = new DesertClient(service_name, type_supports, generate_gid());
  ret->data = (void *)cli;
  
  DesertNode * nd = static_cast<DesertNode *>(node->data);
  nd->add_client(cli);
  
  return ret;
}

rmw_guard_condition_t * rmw_create_guard_condition(rmw_context_t * context)
{
  DEBUG("rmw_create_guard_condition" "\n");
  (void) context;
  
  rmw_guard_condition_t * ret = rmw_guard_condition_allocate();
  ret->implementation_identifier = rmw_get_implementation_identifier();
  
  DesertGuardCondition * con = new DesertGuardCondition();
  ret->data = (void *)con;
  
  return ret;
}

rmw_node_t * rmw_create_node(rmw_context_t * context, const char * name, const char * namespace_)
{
  DEBUG("rmw_create_node" "\n");
  
  rmw_node_t * node = rmw_node_allocate();
  node->implementation_identifier = rmw_get_implementation_identifier();
  DesertNode * nd = new DesertNode(name, namespace_, generate_gid());
  node->data = (void*)nd;
  node->context = context;

  const size_t namelen = strlen(name) + 1;
  node->name = (const char*)malloc(namelen);
  memcpy((char*)node->name, name, namelen);

  const size_t nslen = strlen(namespace_) + 1;
  node->namespace_ = (const char*)malloc(nslen);
  memcpy((char*)node->namespace_, namespace_, nslen);
  
  context->impl->common.graph_cache.add_participant(context->impl->common.gid, "");
  rmw_ret_t ret = Discovery::discovery_thread_start(context->impl);
  (void) ret;

  return node;
}

rmw_publisher_t * rmw_create_publisher(const rmw_node_t * node, const rosidl_message_type_support_t * type_supports, const char * topic_name, const rmw_qos_profile_t * qos_profile, const rmw_publisher_options_t * publisher_options)
{
  DEBUG("rmw_create_publisher" "\n");
  (void) qos_profile;
  (void) publisher_options;
  
  if (strcmp(topic_name, "/discovery") == 0 || strcmp(topic_name, "/discovery_request") == 0)
  {
    RMW_SET_ERROR_MSG("Client applications are not allowed to use '/discovery' and '/discovery_request' topic names");
    return nullptr;
  }

  rmw_publisher_t * ret = rmw_publisher_allocate();
  ret->implementation_identifier = rmw_get_implementation_identifier();
  ret->topic_name = topic_name;
  
  DesertPublisher * pub = new DesertPublisher(topic_name, type_supports, generate_gid());
  ret->data = (void *)pub;
  
  DesertNode * nd = static_cast<DesertNode *>(node->data);
  nd->add_publisher(pub);
  
  return ret;
}

rmw_service_t * rmw_create_service(const rmw_node_t * node, const rosidl_service_type_support_t * type_supports, const char * service_name, const rmw_qos_profile_t * qos_profile)
{
  DEBUG("rmw_create_service" "\n");
  (void) qos_profile;
  
  if (strcmp(service_name, "/discovery") == 0 || strcmp(service_name, "/discovery_request") == 0)
  {
    RMW_SET_ERROR_MSG("Client applications are not allowed to use '/discovery' and '/discovery_request' topic names");
    return nullptr;
  }

  rmw_service_t * ret = rmw_service_allocate();
  ret->implementation_identifier = rmw_get_implementation_identifier();
  ret->service_name = service_name;
  
  DesertService * ser = new DesertService(service_name, type_supports, generate_gid());
  ret->data = (void *)ser;
  
  DesertNode * nd = static_cast<DesertNode *>(node->data);
  nd->add_service(ser);
  
  return ret;
}

rmw_subscription_t * rmw_create_subscription(const rmw_node_t * node, const rosidl_message_type_support_t * type_supports, const char * topic_name, const rmw_qos_profile_t * qos_policies, const rmw_subscription_options_t * subscription_options)
{
  DEBUG("rmw_create_subscription" "\n");
  (void) qos_policies;
  (void) subscription_options;
  
  if (strcmp(topic_name, "/discovery") == 0 || strcmp(topic_name, "/discovery_request") == 0)
  {
    RMW_SET_ERROR_MSG("Client applications are not allowed to use '/discovery' and '/discovery_request' topic names");
    return nullptr;
  }

  rmw_subscription_t * ret = rmw_subscription_allocate();
  ret->implementation_identifier = rmw_get_implementation_identifier();
  ret->topic_name = topic_name;
  
  DesertSubscriber* sub = new DesertSubscriber(topic_name, type_supports, generate_gid());
  ret->data = (void*)sub;
  
  DesertNode * nd = static_cast<DesertNode *>(node->data);
  nd->add_subscriber(sub);
  
  return ret;
}

rmw_wait_set_t * rmw_create_wait_set(rmw_context_t * context,  size_t max_conditions)
{
  DEBUG("rmw_create_wait_set" "\n");
  (void) context;
  (void) max_conditions;

  rmw_wait_set_t * ret = rmw_wait_set_allocate();
  ret->implementation_identifier = rmw_get_implementation_identifier();
  
  DesertWaitset* ws = new DesertWaitset();
  ret->data = (void*)ws;
  
  return ret;
}

rmw_ret_t rmw_deserialize(const rmw_serialized_message_t * serialized_message, const rosidl_message_type_support_t * type_support, void * ros_message)
{
  DEBUG("rmw_deserialize" "\n");
  (void) serialized_message;
  (void) type_support;
  (void) ros_message;
  return RMW_RET_OK;
}

rmw_ret_t rmw_destroy_client(rmw_node_t * node,  rmw_client_t * client)
{
  DEBUG("rmw_destroy_client" "\n");
  
  DesertClient * cli = static_cast<DesertClient *>(client->data);
  
  DesertNode * nd = static_cast<DesertNode *>(node->data);
  nd->remove_client(cli);
  
  delete cli;
  delete client;
  
  return RMW_RET_OK;
}

rmw_ret_t rmw_destroy_guard_condition(rmw_guard_condition_t * guard_condition)
{
  DEBUG("rmw_destroy_guard_condition" "\n");
  
  DesertGuardCondition * con = static_cast<DesertGuardCondition *>(guard_condition->data);
  
  delete con;
  delete guard_condition;
  
  return RMW_RET_OK;
}

rmw_ret_t rmw_destroy_node(rmw_node_t * node)
{
  DEBUG("rmw_destroy_node" "\n");
  
  DesertNode * nod = static_cast<DesertNode *>(node->data);
  
  delete nod;
  delete node;
  
  return RMW_RET_OK;
}

rmw_ret_t rmw_destroy_publisher(rmw_node_t * node,  rmw_publisher_t * publisher)
{
  DEBUG("rmw_destroy_publisher" "\n");
  
  DesertPublisher * pub = static_cast<DesertPublisher *>(publisher->data);
  
  DesertNode * nd = static_cast<DesertNode *>(node->data);
  nd->remove_publisher(pub);
  
  delete pub;
  delete publisher;
  
  return RMW_RET_OK;
}

rmw_ret_t rmw_destroy_service(rmw_node_t * node,  rmw_service_t * service)
{
  DEBUG("rmw_destroy_service" "\n");
  
  DesertService * ser = static_cast<DesertService *>(service->data);
  
  DesertNode * nd = static_cast<DesertNode *>(node->data);
  nd->remove_service(ser);
  
  delete ser;
  delete service;
  
  return RMW_RET_OK;
}

rmw_ret_t rmw_destroy_subscription(rmw_node_t * node,  rmw_subscription_t * subscription)
{
  DEBUG("rmw_destroy_subscription" "\n");
  
  DesertSubscriber * sub = static_cast<DesertSubscriber *>(subscription->data);
  
  DesertNode * nd = static_cast<DesertNode *>(node->data);
  nd->remove_subscriber(sub);
  
  delete sub;
  delete subscription;
  
  return RMW_RET_OK;
}

rmw_ret_t rmw_destroy_wait_set(rmw_wait_set_t * wait_set)
{
  DEBUG("rmw_destroy_wait_set" "\n");
  
  DesertWaitset * ws = static_cast<DesertWaitset *>(wait_set->data);
  
  delete ws;
  delete wait_set;
  
  return RMW_RET_OK;
}

rmw_ret_t rmw_event_set_callback(rmw_event_t * event, rmw_event_callback_t callback, const void * user_data)
{
  DEBUG("rmw_event_set_callback" "\n");
  (void) event;
  (void) callback;
  (void) user_data;
  return RMW_RET_OK;
}

rmw_ret_t rmw_fini_publisher_allocation(rmw_publisher_allocation_t * allocation)
{
  DEBUG("rmw_fini_publisher_allocation" "\n");
  (void) allocation;
  return RMW_RET_OK;
}

rmw_ret_t rmw_fini_subscription_allocation(rmw_subscription_allocation_t * allocation)
{
  DEBUG("rmw_fini_subscription_allocation" "\n");
  (void) allocation;
  return RMW_RET_OK;
}

rmw_ret_t rmw_get_gid_for_client(const rmw_client_t * client,  rmw_gid_t * gid)
{
  DEBUG("rmw_get_gid_for_client" "\n");
  
  DesertClient * cli = static_cast<DesertClient *>(client->data);
  
  rmw_gid_t * id = new rmw_gid_t(cli->get_gid());
  
  memset(gid->data, 0, RMW_GID_STORAGE_SIZE);
  memcpy(gid->data, id, RMW_GID_STORAGE_SIZE);
  
  return RMW_RET_OK;
}

rmw_ret_t rmw_get_gid_for_publisher(const rmw_publisher_t * publisher,  rmw_gid_t * gid)
{
  DEBUG("rmw_get_gid_for_publisher" "\n");
  
  DesertPublisher * pub = static_cast<DesertPublisher *>(publisher->data);
  
  rmw_gid_t * id = new rmw_gid_t(pub->get_gid());
  
  memset(gid->data, 0, RMW_GID_STORAGE_SIZE);
  memcpy(gid->data, id, RMW_GID_STORAGE_SIZE);
  
  return RMW_RET_OK;
}

const char * rmw_get_implementation_identifier(void)
{
  static const char * fake_impl_id = "rmw_desert";
  return fake_impl_id;
}

rmw_ret_t rmw_get_node_names(const rmw_node_t * node, rcutils_string_array_t * node_names, rcutils_string_array_t * node_namespaces)
{
  DEBUG("rmw_get_node_names" "\n");

  auto common_context = &node->context->impl->common;
  rcutils_allocator_t allocator = rcutils_get_default_allocator();

  return common_context->graph_cache.get_node_names(
    node_names,
    node_namespaces,
    nullptr,
    &allocator);
}

rmw_ret_t rmw_get_node_names_with_enclaves(const rmw_node_t * node, rcutils_string_array_t * node_names, rcutils_string_array_t * node_namespaces, rcutils_string_array_t * enclaves)
{
  DEBUG("rmw_get_node_names_with_enclaves" "\n");
  (void) node;
  (void) node_names;
  (void) node_namespaces;
  (void) enclaves;
  return RMW_RET_OK;
}

const char * rmw_get_serialization_format(void)
{
  DEBUG("rmw_get_serialization_format" "\n");
  return "";
}

rmw_ret_t rmw_get_serialized_message_size(const rosidl_message_type_support_t * type_support, const rosidl_runtime_c__Sequence__bound * message_bounds, size_t * size)
{
  DEBUG("rmw_get_serialized_message_size" "\n");
  (void) type_support;
  (void) message_bounds;
  (void) size;
  return RMW_RET_OK;
}

rmw_ret_t rmw_init_publisher_allocation(const rosidl_message_type_support_t * type_support, const rosidl_runtime_c__Sequence__bound * message_bounds, rmw_publisher_allocation_t * allocation)
{
  DEBUG("rmw_init_publisher_allocation" "\n");
  (void) type_support;
  (void) message_bounds;
  (void) allocation;
  return RMW_RET_OK;
}

rmw_ret_t rmw_init_subscription_allocation(const rosidl_message_type_support_t * type_support, const rosidl_runtime_c__Sequence__bound * message_bounds, rmw_subscription_allocation_t * allocation)
{
  DEBUG("rmw_init_subscription_allocation" "\n");
  (void) type_support;
  (void) message_bounds;
  (void) allocation;
  return RMW_RET_OK;
}

rmw_ret_t rmw_node_assert_liveliness(const rmw_node_t * node)
{
  DEBUG("rmw_node_assert_liveliness" "\n");
  (void) node;
  return RMW_RET_OK;
}

const rmw_guard_condition_t * rmw_node_get_graph_guard_condition(const rmw_node_t * node)
{
  DEBUG("rmw_node_get_graph_guard_condition" "\n");
  (void) node;
  
  rmw_guard_condition_t * ret = rmw_guard_condition_allocate();
  ret->data = NULL;
  ret->implementation_identifier = rmw_get_implementation_identifier();
  
  return ret;
}

rmw_ret_t rmw_publish(const rmw_publisher_t * publisher, const void * ros_message, rmw_publisher_allocation_t * allocation)
{
  DEBUG("rmw_publish" "\n");
  (void) allocation;
  
  DesertPublisher * pub = static_cast<DesertPublisher *>(publisher->data);
  pub->push(ros_message);
  
  return RMW_RET_OK;
}

rmw_ret_t rmw_publish_loaned_message(const rmw_publisher_t * publisher, void * ros_message, rmw_publisher_allocation_t * allocation)
{
  DEBUG("rmw_publish_loaned_message" "\n");
  (void) publisher;
  (void) ros_message;
  (void) allocation;
  return RMW_RET_OK;
}

rmw_ret_t rmw_publish_serialized_message(const rmw_publisher_t * publisher, const rmw_serialized_message_t * serialized_message, rmw_publisher_allocation_t * allocation)
{
  DEBUG("rmw_publish_serialized_message" "\n");
  (void) publisher;
  (void) serialized_message;
  (void) allocation;
  return RMW_RET_OK;
}

rmw_ret_t rmw_publisher_assert_liveliness(const rmw_publisher_t * publisher)
{
  DEBUG("rmw_publisher_assert_liveliness" "\n");
  (void) publisher;
  return RMW_RET_OK;
}

rmw_ret_t rmw_publisher_count_matched_subscriptions(const rmw_publisher_t * publisher, size_t * subscription_count)
{
  DEBUG("rmw_publisher_count_matched_subscriptions" "\n");
  (void) publisher;
  (void) subscription_count;
  return RMW_RET_OK;
}

rmw_ret_t rmw_publisher_get_actual_qos(const rmw_publisher_t * publisher, rmw_qos_profile_t * qos)
{
  DEBUG("rmw_publisher_get_actual_qos" "\n");
  (void) publisher;
  (void) qos;
  return RMW_RET_OK;
}

rmw_ret_t rmw_publisher_wait_for_all_acked(const rmw_publisher_t * publisher, rmw_time_t wait_timeout)
{
  DEBUG("rmw_publisher_wait_for_all_acked" "\n");
  (void) publisher;
  (void) wait_timeout;
  return RMW_RET_OK;
}

rmw_ret_t rmw_return_loaned_message_from_publisher(const rmw_publisher_t * publisher, void * loaned_message)
{
  DEBUG("rmw_return_loaned_message_from_publisher" "\n");
  (void) publisher;
  (void) loaned_message;
  return RMW_RET_OK;
}

rmw_ret_t rmw_return_loaned_message_from_subscription(const rmw_subscription_t * subscription, void * loaned_message)
{
  DEBUG("rmw_return_loaned_message_from_subscription" "\n");
  (void) subscription;
  (void) loaned_message;
  return RMW_RET_OK;
}

rmw_ret_t rmw_send_request(const rmw_client_t * client, const void * ros_request, int64_t * sequence_id)
{
  DEBUG("rmw_send_request" "\n");
  
  *sequence_id = rand() % 1000;
  
  DesertClient * cli = static_cast<DesertClient *>(client->data);
  cli->send_request(ros_request, sequence_id);
  
  return RMW_RET_OK;
}

rmw_ret_t rmw_send_response(const rmw_service_t * service, rmw_request_id_t * request_header, void * ros_response)
{
  DEBUG("rmw_send_response" "\n");
  
  DesertService * ser = static_cast<DesertService *>(service->data);
  ser->send_response(ros_response, request_header);
  
  return RMW_RET_OK;
}

rmw_ret_t rmw_serialize(const void * ros_message, const rosidl_message_type_support_t * type_support, rmw_serialized_message_t * serialized_message)
{
  DEBUG("rmw_serialize" "\n");
  (void) ros_message;
  (void) type_support;
  (void) serialized_message;
  return RMW_RET_OK;
}

rmw_ret_t rmw_service_request_subscription_get_actual_qos(const rmw_service_t * service, rmw_qos_profile_t * qos)
{
  DEBUG("rmw_service_request_subscription_get_actual_qos" "\n");
  (void) service;
  (void) qos;
  return RMW_RET_OK;
}

rmw_ret_t rmw_service_response_publisher_get_actual_qos(const rmw_service_t * service, rmw_qos_profile_t * qos)
{
  DEBUG("rmw_service_response_publisher_get_actual_qos" "\n");
  (void) service;
  (void) qos;
  return RMW_RET_OK;
}

rmw_ret_t rmw_service_server_is_available(const rmw_node_t * node, const rmw_client_t * client, bool * is_available)
{
  DEBUG("rmw_service_server_is_available" "\n");
  
  size_t * count = new size_t();
  rmw_ret_t ret = rmw_count_services(node, client->service_name, count);
  
  *is_available = *count > 0;
  
  return ret;
}

rmw_ret_t rmw_service_set_on_new_request_callback(rmw_service_t * service, rmw_event_callback_t callback, const void * user_data)
{
  DEBUG("rmw_service_set_on_new_request_callback" "\n");
  (void) service;
  (void) callback;
  (void) user_data;
  return RMW_RET_OK;
}

rmw_ret_t rmw_set_log_severity(rmw_log_severity_t severity)
{
  DEBUG("rmw_set_log_severity" "\n");
  (void) severity;
  return RMW_RET_OK;
}

rmw_ret_t rmw_subscription_count_matched_publishers(const rmw_subscription_t * subscription, size_t * publisher_count)
{
  DEBUG("rmw_subscription_count_matched_publishers" "\n");
  (void) subscription;
  (void) publisher_count;
  return RMW_RET_OK;
}

rmw_ret_t rmw_subscription_get_actual_qos(const rmw_subscription_t * subscription, rmw_qos_profile_t * qos)
{
  DEBUG("rmw_subscription_get_actual_qos" "\n");
  (void) subscription;
  (void) qos;
  return RMW_RET_OK;
}

rmw_ret_t rmw_subscription_get_content_filter(const rmw_subscription_t * subscription, rcutils_allocator_t * allocator, rmw_subscription_content_filter_options_t * options)
{
  DEBUG("rmw_subscription_get_content_filter" "\n");
  (void) subscription;
  (void) allocator;
  (void) options;
  return RMW_RET_OK;
}

rmw_ret_t rmw_subscription_set_content_filter(rmw_subscription_t * subscription, const rmw_subscription_content_filter_options_t * options)
{
  DEBUG("rmw_subscription_set_content_filter" "\n");
  (void) subscription;
  (void) options;
  return RMW_RET_OK;
}

rmw_ret_t rmw_subscription_set_on_new_message_callback(rmw_subscription_t * subscription, rmw_event_callback_t callback, const void * user_data)
{
  DEBUG("rmw_subscription_set_on_new_message_callback" "\n");
  (void) subscription;
  (void) callback;
  (void) user_data;
  return RMW_RET_OK;
}

rmw_ret_t rmw_take(const rmw_subscription_t * subscription, void * ros_message, bool * taken, rmw_subscription_allocation_t * allocation)
{
  DEBUG("rmw_take" "\n");
  (void) allocation;
  
  DesertSubscriber * sub = static_cast<DesertSubscriber *>(subscription->data);
  if (sub->has_data())
  {
    sub->read_data(ros_message);
    *taken = true;
  }
  
  usleep(1000);
  return RMW_RET_OK;
}

rmw_ret_t rmw_take_loaned_message(const rmw_subscription_t * subscription, void ** loaned_message, bool * taken, rmw_subscription_allocation_t * allocation)
{
  DEBUG("rmw_take_loaned_message" "\n");
  (void) subscription;
  (void) loaned_message;
  (void) taken;
  (void) allocation;
  return RMW_RET_OK;
}

rmw_ret_t rmw_take_loaned_message_with_info(const rmw_subscription_t * subscription, void ** loaned_message, bool * taken, rmw_message_info_t * message_info, rmw_subscription_allocation_t * allocation)
{
  DEBUG("rmw_take_loaned_message_with_info" "\n");
  (void) subscription;
  (void) loaned_message;
  (void) taken;
  (void) message_info;
  (void) allocation;
  return RMW_RET_OK;
}

rmw_ret_t rmw_take_request(const rmw_service_t * service, rmw_service_info_t * request_header, void * ros_request, bool * taken)
{
  DEBUG("rmw_take_request" "\n");
  
  DesertService * ser = static_cast<DesertService *>(service->data);
  if (ser->has_data())
  {
    ser->read_request(ros_request, request_header);
    *taken = true;
  }
  
  usleep(1000);
  return RMW_RET_OK;
}

rmw_ret_t rmw_take_response(const rmw_client_t * client, rmw_service_info_t * request_header, void * ros_response, bool * taken)
{
  DEBUG("rmw_take_response" "\n");
  
  DesertClient * cli = static_cast<DesertClient *>(client->data);
  if (cli->has_data())
  {
    cli->read_response(ros_response, request_header);
    *taken = true;
  }
  
  usleep(1000);
  return RMW_RET_OK;
}

rmw_ret_t rmw_take_sequence(const rmw_subscription_t * subscription, size_t count, rmw_message_sequence_t * message_sequence, rmw_message_info_sequence_t * message_info_sequence, size_t * taken, rmw_subscription_allocation_t * allocation)
{
  DEBUG("rmw_take_sequence" "\n");
  (void) subscription;
  (void) count;
  (void) message_sequence;
  (void) message_info_sequence;
  (void) taken;
  (void) allocation;
  return RMW_RET_OK;
}

rmw_ret_t rmw_take_serialized_message(const rmw_subscription_t * subscription, rmw_serialized_message_t * serialized_message, bool * taken, rmw_subscription_allocation_t * allocation)
{
  DEBUG("rmw_take_serialized_message" "\n");
  (void) subscription;
  (void) serialized_message;
  (void) taken;
  (void) allocation;
  return RMW_RET_OK;
}

rmw_ret_t rmw_take_serialized_message_with_info(const rmw_subscription_t * subscription, rmw_serialized_message_t * serialized_message, bool * taken, rmw_message_info_t * message_info, rmw_subscription_allocation_t * allocation)
{
  DEBUG("rmw_take_serialized_message_with_info" "\n");
  (void) subscription;
  (void) serialized_message;
  (void) taken;
  (void) message_info;
  (void) allocation;
  return RMW_RET_OK;
}

rmw_ret_t rmw_take_with_info(const rmw_subscription_t * subscription, void * ros_message, bool * taken, rmw_message_info_t * message_info, rmw_subscription_allocation_t * allocation)
{
  DEBUG("rmw_take_with_info" "\n");
  (void) message_info;
  (void) allocation;
  
  DesertSubscriber * sub = static_cast<DesertSubscriber *>(subscription->data);
  if (sub->has_data())
  {
    sub->read_data(ros_message);
    *taken = true;
  }
  
  usleep(1000);
  return RMW_RET_OK;
}

rmw_ret_t rmw_trigger_guard_condition(const rmw_guard_condition_t * guard_condition)
{
  DEBUG("rmw_trigger_guard_condition" "\n");
  
  DesertGuardCondition * con = static_cast<DesertGuardCondition *>(guard_condition->data);
  con->trigger();
  
  return RMW_RET_OK;
}

rmw_ret_t rmw_wait(rmw_subscriptions_t * subscriptions, rmw_guard_conditions_t * guard_conditions, rmw_services_t * services, rmw_clients_t * clients, rmw_events_t * events, rmw_wait_set_t * wait_set, const rmw_time_t * wait_timeout)
{
  DEBUG("rmw_wait" "\n");
  (void) wait_set;
  (void) wait_timeout;
  
  usleep(1000);

  if (subscriptions)
  {
    for (size_t i = 0; i < subscriptions->subscriber_count; ++i)
    {
      void * data = subscriptions->subscribers[i];
      DesertSubscriber * sub = static_cast<DesertSubscriber *>(data);
      if (!sub->has_data())
      {
        subscriptions->subscribers[i] = nullptr;
      }
    }
  }

  if (clients)
  {
    for (size_t i = 0; i < clients->client_count; ++i)
    {
      void * data = clients->clients[i];
      DesertClient * cli = static_cast<DesertClient *>(data);
      if (!cli->has_data()) 
      {
        clients->clients[i] = nullptr;
      }
    }
  }

  if (services)
  {
    for (size_t i = 0; i < services->service_count; ++i)
    {
      void * data = services->services[i];
      DesertService * ser = static_cast<DesertService *>(data);
      if (!ser->has_data())
      {
        services->services[i] = nullptr;
      }
    }
  }

  if (guard_conditions)
  {
    for (size_t i = 0; i < guard_conditions->guard_condition_count; ++i)
    {
      void * data = guard_conditions->guard_conditions[i];
      DesertGuardCondition * con = static_cast<DesertGuardCondition *>(data);
      if (!con->get_has_triggered())
      {
        guard_conditions->guard_conditions[i] = nullptr;
      }
    }
  }
  
  if (events)
  {
    for (size_t i = 0; i < events->event_count; i++)
    {
      events->events[i] = nullptr;
    }
  }
  
  return RMW_RET_OK;
}

