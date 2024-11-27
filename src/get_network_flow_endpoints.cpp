#include "rmw/get_network_flow_endpoints.h"
#include "debug.h"


rmw_ret_t rmw_publisher_get_network_flow_endpoints(const rmw_publisher_t * publisher, rcutils_allocator_t * allocator, rmw_network_flow_endpoint_array_t * network_flow_endpoint_array)
{
  DEBUG("rmw_publisher_get_network_flow_endpoints" "\n");
  (void) publisher;
  (void) allocator;
  (void) network_flow_endpoint_array;
  RMW_SET_ERROR_MSG("rmw_publisher_get_network_flow_endpoints not implemented");
  return RMW_RET_UNSUPPORTED;
}

rmw_ret_t rmw_subscription_get_network_flow_endpoints(const rmw_subscription_t * subscription, rcutils_allocator_t * allocator, rmw_network_flow_endpoint_array_t * network_flow_endpoint_array)
{
  DEBUG("rmw_subscription_get_network_flow_endpoints" "\n");
  (void) subscription;
  (void) allocator;
  (void) network_flow_endpoint_array;
  RMW_SET_ERROR_MSG("rmw_subscription_get_network_flow_endpoints not implemented");
  return RMW_RET_UNSUPPORTED;
}

