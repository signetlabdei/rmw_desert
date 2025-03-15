#include "rmw/init.h"
#include "classes.h"
#include "debug.h"

int get_desert_port()
{
  const char * env_variable = "DESERT_PORT";
  char * value;

  value = getenv(env_variable);

  if (value != NULL && atoi(value) > 0)
  {
    return atoi(value);
  }
  else
  {
    printf("No or invalid DESERT port was set... defaulting to 4000\n");
    return 4000;
  }
}

rmw_ret_t rmw_context_fini(rmw_context_t * context)
{
  DEBUG("rmw_context_fini" "\n");
  // Sleep needed to allow TcpDaemon sending disconnection beacons
  usleep(100000);
  
  if (!context->impl->is_shutdown)
  {
    RMW_SET_ERROR_MSG("context has not been shutdown");
    return RMW_RET_INVALID_ARGUMENT;
  }

  delete context->impl;
  *context = rmw_get_zero_initialized_context();
  
  return RMW_RET_OK;
}

rmw_ret_t rmw_init(const rmw_init_options_t * options,  rmw_context_t * context)
{
  DEBUG("rmw_init" "\n");
  srand (time(NULL));
  
  int port = get_desert_port();
  
  TopicsConfig::load_configuration();
  TcpDaemon tcp = TcpDaemon();
  
  auto restore_context = rcpputils::make_scope_exit(
    [context]() {*context = rmw_get_zero_initialized_context();});
  
  context->instance_id = options->instance_id;
  context->implementation_identifier = rmw_get_implementation_identifier();
  context->actual_domain_id = 0u;
  context->impl = new rmw_context_impl_t();
  
  context->impl->common.graph_cache.add_participant(context->impl->common.gid, "");
  rmw_ret_t ret = Discovery::discovery_thread_start(context->impl);
  
  if (ret != RMW_RET_OK)
    return RMW_RET_ERROR;
  
  if (tcp.init(port) != 0)
    return RMW_RET_ERROR;
  
  restore_context.cancel();
  return RMW_RET_OK;
}

rmw_ret_t rmw_shutdown(rmw_context_t * context)
{
  DEBUG("rmw_shutdown" "\n");
  
  context->impl->is_shutdown = true;
  rmw_ret_t ret = Discovery::discovery_thread_stop(context->impl);
  
  if (ret != RMW_RET_OK)
    return ret;
  
  return RMW_RET_OK;
}

