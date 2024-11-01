#include "rmw/init.h"
#include "classes.h"
#include "debug.h"

int get_desert_port()
{
  char* env_variable = "DESERT_PORT";
  char* value;

  value = getenv(env_variable);

  if (value != NULL && atoi(value) > 0)
  {
    printf("Connecting to DESERT through the TCP port %d\n", atoi(value));
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
  return RMW_RET_OK;
}

rmw_ret_t rmw_init(const rmw_init_options_t * options,  rmw_context_t * context)
{
  DEBUG("rmw_init" "\n");
  srand (time(NULL));
  
  int port = get_desert_port();
  
  TopicsConfig::load_configuration();
  TcpDaemon tcp = TcpDaemon();
  
  if (tcp.init(port) == 0)
    return RMW_RET_OK;
  else
    return RMW_RET_ERROR;
}

rmw_ret_t rmw_shutdown(rmw_context_t * context)
{
  DEBUG("rmw_shutdown" "\n");
  return RMW_RET_OK;
}

