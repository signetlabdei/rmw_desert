#include "rmw/get_service_names_and_types.h"
#include "classes.h"
#include "debug.h"


rmw_ret_t rmw_get_service_names_and_types(const rmw_node_t * node, rcutils_allocator_t * allocator, rmw_names_and_types_t * service_names_and_types)
{
  DEBUG("rmw_get_service_names_and_types" "\n");
  
  auto common_context = &node->context->impl->common;
  
  return common_context->graph_cache.get_names_and_types(
    Discovery::demangle_service_from_topic,    // Demangle topic
    Discovery::identity_demangle,              // Demangle type
    allocator,
    service_names_and_types);
}

