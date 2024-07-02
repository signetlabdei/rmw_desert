#include <stdio.h>

#define DEBUG_ENABLED 1

#if DEBUG_ENABLED
#define DEBUG(...) printf(__VA_ARGS__)
#else
#define DEBUG(...) 
#endif
