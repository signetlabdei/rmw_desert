#include "rmw/get_node_info_and_types.h"
#include "classes.h"
#include "debug.h"


rmw_ret_t rmw_get_client_names_and_types_by_node(const rmw_node_t * node, rcutils_allocator_t * allocator, const char * node_name, const char * node_namespace, rmw_names_and_types_t * service_names_and_types)
{
  DEBUG("rmw_get_client_names_and_types_by_node" "\n");
  
  auto common_context = &node->context->impl->common;
  
  return common_context->graph_cache.get_reader_names_and_types_by_node(
    node_name,
    node_namespace,
    Discovery::demangle_service_request_from_topic,
    Discovery::identity_demangle,
    allocator,
    service_names_and_types);
}

rmw_ret_t rmw_get_publisher_names_and_types_by_node(const rmw_node_t * node, rcutils_allocator_t * allocator, const char * node_name, const char * node_namespace, bool no_demangle, rmw_names_and_types_t * topic_names_and_types)
{
  DEBUG("rmw_get_publisher_names_and_types_by_node" "\n");
  (void) no_demangle;
  
  auto common_context = &node->context->impl->common;
  
  return common_context->graph_cache.get_writer_names_and_types_by_node(
    node_name,
    node_namespace,
    Discovery::demangle_publisher_from_topic,
    Discovery::identity_demangle,
    allocator,
    topic_names_and_types);
}

rmw_ret_t rmw_get_service_names_and_types_by_node(const rmw_node_t * node, rcutils_allocator_t * allocator, const char * node_name, const char * node_namespace, rmw_names_and_types_t * service_names_and_types)
{
  DEBUG("rmw_get_service_names_and_types_by_node" "\n");
  
  auto common_context = &node->context->impl->common;
  
  return common_context->graph_cache.get_reader_names_and_types_by_node(
    node_name,
    node_namespace,
    Discovery::demangle_service_reply_from_topic,
    Discovery::identity_demangle,
    allocator,
    service_names_and_types);
}

rmw_ret_t rmw_get_subscriber_names_and_types_by_node(const rmw_node_t * node, rcutils_allocator_t * allocator, const char * node_name, const char * node_namespace, bool no_demangle, rmw_names_and_types_t * topic_names_and_types)
{
  DEBUG("rmw_get_subscriber_names_and_types_by_node" "\n");
  (void) no_demangle;
  
  auto common_context = &node->context->impl->common;
  
  return common_context->graph_cache.get_reader_names_and_types_by_node(
    node_name,
    node_namespace,
    Discovery::demangle_subscriber_from_topic,
    Discovery::identity_demangle,
    allocator,
    topic_names_and_types);
}

