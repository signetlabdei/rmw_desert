#include <stdio.h>
#include "rmw/impl/cpp/macros.hpp"

#define DEBUG_ENABLED 0

#if DEBUG_ENABLED
#define DEBUG(...) printf(__VA_ARGS__)
#else
#define DEBUG(...) 
#endif
