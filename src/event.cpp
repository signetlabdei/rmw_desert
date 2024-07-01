#include "rmw/event.h"


rmw_ret_t rmw_publisher_event_init(rmw_event_t * rmw_event, const rmw_publisher_t * publisher, rmw_event_type_t event_type)
{
  return RMW_RET_OK;
}

rmw_ret_t rmw_subscription_event_init(rmw_event_t * rmw_event, const rmw_subscription_t * subscription, rmw_event_type_t event_type)
{
  return RMW_RET_OK;
}

rmw_ret_t rmw_take_event(const rmw_event_t * event_handle, void * event_info, bool * taken)
{
  return RMW_RET_OK;
}

