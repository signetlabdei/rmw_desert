#include "rmw/get_topic_endpoint_info.h"
#include "debug.h"


rmw_ret_t rmw_get_publishers_info_by_topic(const rmw_node_t * node, rcutils_allocator_t * allocator, const char * topic_name, bool no_mangle, rmw_topic_endpoint_info_array_t * publishers_info)
{
  DEBUG("rmw_get_publishers_info_by_topic" "\n");
  return RMW_RET_OK;
}

rmw_ret_t rmw_get_subscriptions_info_by_topic(const rmw_node_t * node, rcutils_allocator_t * allocator, const char * topic_name, bool no_mangle, rmw_topic_endpoint_info_array_t * subscriptions_info)
{
  DEBUG("rmw_get_subscriptions_info_by_topic" "\n");
  return RMW_RET_OK;
}

