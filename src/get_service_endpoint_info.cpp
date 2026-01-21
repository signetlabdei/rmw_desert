#include "rmw/get_service_endpoint_info.h"
#include "debug.h"


rmw_ret_t rmw_get_clients_info_by_service(const rmw_node_t * node, rcutils_allocator_t * allocator, const char * service_name, bool no_mangle, rmw_service_endpoint_info_array_t * clients_info)
{
  DEBUG("rmw_get_clients_info_by_service" "\n");
  (void) node;
  (void) allocator;
  (void) service_name;
  (void) no_mangle;
  (void) clients_info;
  RMW_SET_ERROR_MSG("rmw_get_clients_info_by_service not implemented");
  return RMW_RET_UNSUPPORTED;
}

rmw_ret_t rmw_get_servers_info_by_service(const rmw_node_t * node, rcutils_allocator_t * allocator, const char * service_name, bool no_mangle, rmw_service_endpoint_info_array_t * servers_info)
{
  DEBUG("rmw_get_servers_info_by_service" "\n");
  (void) node;
  (void) allocator;
  (void) service_name;
  (void) no_mangle;
  (void) servers_info;
  RMW_SET_ERROR_MSG("rmw_get_servers_info_by_service not implemented");
  return RMW_RET_UNSUPPORTED;
}

