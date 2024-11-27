#include "rmw/get_topic_names_and_types.h"
#include "classes.h"
#include "debug.h"


rmw_ret_t rmw_get_topic_names_and_types(const rmw_node_t * node, rcutils_allocator_t * allocator, bool no_demangle, rmw_names_and_types_t * topic_names_and_types)
{
  DEBUG("rmw_get_topic_names_and_types" "\n");
  
  auto common_context = &node->context->impl->common;
  
  return common_context->graph_cache.get_names_and_types(
    Discovery::demangle_topic,                 // Demangle topic
    Discovery::identity_demangle,              // Demangle type
    allocator,
    topic_names_and_types);
}

