#include "rmw/get_node_info_and_types.h"


rmw_ret_t rmw_get_client_names_and_types_by_node(const rmw_node_t * node, rcutils_allocator_t * allocator, const char * node_name, const char * node_namespace, rmw_names_and_types_t * service_names_and_types)
{
  return RMW_RET_OK;
}

rmw_ret_t rmw_get_publisher_names_and_types_by_node(const rmw_node_t * node, rcutils_allocator_t * allocator, const char * node_name, const char * node_namespace, bool no_demangle, rmw_names_and_types_t * topic_names_and_types)
{
  return RMW_RET_OK;
}

rmw_ret_t rmw_get_service_names_and_types_by_node(const rmw_node_t * node, rcutils_allocator_t * allocator, const char * node_name, const char * node_namespace, rmw_names_and_types_t * service_names_and_types)
{
  return RMW_RET_OK;
}

rmw_ret_t rmw_get_subscriber_names_and_types_by_node(const rmw_node_t * node, rcutils_allocator_t * allocator, const char * node_name, const char * node_namespace, bool no_demangle, rmw_names_and_types_t * topic_names_and_types)
{
  return RMW_RET_OK;
}

