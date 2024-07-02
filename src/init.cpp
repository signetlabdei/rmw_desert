#include "rmw/init.h"
#include "debug.h"


rmw_ret_t rmw_context_fini(rmw_context_t * context)
{
  DEBUG("rmw_context_fini" "\n");
  return RMW_RET_OK;
}

rmw_ret_t rmw_init(const rmw_init_options_t * options,  rmw_context_t * context)
{
  DEBUG("rmw_init" "\n");
  return RMW_RET_OK;
}

rmw_ret_t rmw_shutdown(rmw_context_t * context)
{
  DEBUG("rmw_shutdown" "\n");
  return RMW_RET_OK;
}

