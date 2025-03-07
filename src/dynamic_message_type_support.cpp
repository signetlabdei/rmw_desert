#include "rmw/dynamic_message_type_support.h"
#include "debug.h"


rmw_ret_t rmw_serialization_support_init(const char * serialization_lib_name, rcutils_allocator_t * allocator, rosidl_dynamic_typesupport_serialization_support_t * serialization_support)
{
  DEBUG("rmw_serialization_support_init" "\n");
  (void) serialization_lib_name;
  (void) allocator;
  (void) serialization_support;
  RMW_SET_ERROR_MSG("rmw_serialization_support_init not implemented");
  return RMW_RET_UNSUPPORTED;
}

rmw_ret_t rmw_take_dynamic_message(const rmw_subscription_t * subscription, rosidl_dynamic_typesupport_dynamic_data_t * dynamic_message, bool * taken, rmw_subscription_allocation_t * allocation)
{
  DEBUG("rmw_take_dynamic_message" "\n");
  (void) subscription;
  (void) dynamic_message;
  (void) taken;
  (void) allocation;
  RMW_SET_ERROR_MSG("rmw_take_dynamic_message not implemented");
  return RMW_RET_UNSUPPORTED;
}

rmw_ret_t rmw_take_dynamic_message_with_info(const rmw_subscription_t * subscription, rosidl_dynamic_typesupport_dynamic_data_t * dynamic_message, bool * taken, rmw_message_info_t * message_info, rmw_subscription_allocation_t * allocation)
{
  DEBUG("rmw_take_dynamic_message_with_info" "\n");
  (void) subscription;
  (void) dynamic_message;
  (void) taken;
  (void) message_info;
  (void) allocation;
  RMW_SET_ERROR_MSG("rmw_take_dynamic_message_with_info not implemented");
  return RMW_RET_UNSUPPORTED;
}

