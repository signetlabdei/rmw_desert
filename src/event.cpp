#include "rmw/event.h"
#include "debug.h"

static bool is_event_supported(const rmw_event_type_t event_t)
{
  // We are saying that all events are supported to prevent rclcpp showing a lot of warnings for this functionality, but they are not really implemented
  return true;
}

static rmw_ret_t init_rmw_event(rmw_event_t * rmw_event, const char * topic_impl_identifier, void * data, rmw_event_type_t event_type)
{
  if (!is_event_supported(event_type)) {
    RMW_SET_ERROR_MSG("provided event_type is not supported by rmw_desert");
    return RMW_RET_UNSUPPORTED;
  }
  rmw_event->implementation_identifier = topic_impl_identifier;
  rmw_event->data = data;
  rmw_event->event_type = event_type;
  return RMW_RET_OK;
}

rmw_ret_t rmw_publisher_event_init(rmw_event_t * rmw_event, const rmw_publisher_t * publisher, rmw_event_type_t event_type)
{
  DEBUG("rmw_publisher_event_init" "\n");
  return init_rmw_event(
    rmw_event,
    publisher->implementation_identifier,
    publisher->data,
    event_type);
}

rmw_ret_t rmw_subscription_event_init(rmw_event_t * rmw_event, const rmw_subscription_t * subscription, rmw_event_type_t event_type)
{
  DEBUG("rmw_subscription_event_init" "\n");
  return init_rmw_event(
    rmw_event,
    subscription->implementation_identifier,
    subscription->data,
    event_type);
}

rmw_ret_t rmw_take_event(const rmw_event_t * event_handle, void * event_info, bool * taken)
{
  RMW_SET_ERROR_MSG("unimplemented");
  return RMW_RET_ERROR;
}

