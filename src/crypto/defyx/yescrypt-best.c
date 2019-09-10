#ifdef __ARM_NEON__
#include "yescrypt-neon.c"
#elif __SSE2__
#include "yescrypt-simd.c"
#else
#include "yescrypt-opt.c"
#endif
