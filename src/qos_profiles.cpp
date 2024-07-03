#include "rmw/qos_profiles.h"
#include "debug.h"


rmw_ret_t rmw_qos_profile_check_compatible(const rmw_qos_profile_t publisher_profile, const rmw_qos_profile_t subscription_profile, rmw_qos_compatibility_type_t * compatibility, char * reason, size_t reason_size)
{
  DEBUG("rmw_qos_profile_check_compatible" "\n");
  return RMW_RET_OK;
}

