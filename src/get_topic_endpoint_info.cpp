#include "rmw/get_topic_endpoint_info.h"
#include "classes.h"
#include "debug.h"


rmw_ret_t rmw_get_publishers_info_by_topic(const rmw_node_t * node, rcutils_allocator_t * allocator, const char * topic_name, bool no_mangle, rmw_topic_endpoint_info_array_t * publishers_info)
{
  DEBUG("rmw_get_publishers_info_by_topic" "\n");
  (void) no_mangle;
  
  auto common_context = &node->context->impl->common;
  const std::string mangled_topic_name = std::string(Discovery::ros_topic_publisher_prefix) + std::string(topic_name);
  
  return common_context->graph_cache.get_writers_info_by_topic(
    mangled_topic_name,
    Discovery::identity_demangle,
    allocator,
    publishers_info);
}

rmw_ret_t rmw_get_subscriptions_info_by_topic(const rmw_node_t * node, rcutils_allocator_t * allocator, const char * topic_name, bool no_mangle, rmw_topic_endpoint_info_array_t * subscriptions_info)
{
  DEBUG("rmw_get_subscriptions_info_by_topic" "\n");
  (void) no_mangle;
  
  auto common_context = &node->context->impl->common;
  const std::string mangled_topic_name = std::string(Discovery::ros_topic_subscriber_prefix) + std::string(topic_name);
  
  return common_context->graph_cache.get_readers_info_by_topic(
    mangled_topic_name,
    Discovery::identity_demangle,
    allocator,
    subscriptions_info);
}

