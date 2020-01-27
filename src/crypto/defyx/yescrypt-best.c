#ifdef __ARM__
#include "yescrypt-neon.c"
#else
#include "yescrypt-opt.c"
#endif
