#include "rmw/init_options.h"
#include "debug.h"


rmw_ret_t rmw_init_options_copy(const rmw_init_options_t * src,  rmw_init_options_t * dst)
{
  DEBUG("rmw_init_options_copy" "\n");
  return RMW_RET_OK;
}

rmw_ret_t rmw_init_options_fini(rmw_init_options_t * init_options)
{
  DEBUG("rmw_init_options_fini" "\n");
  return RMW_RET_OK;
}

rmw_ret_t rmw_init_options_init(rmw_init_options_t * init_options,  rcutils_allocator_t allocator)
{
  DEBUG("rmw_init_options_init" "\n");
  return RMW_RET_OK;
}

