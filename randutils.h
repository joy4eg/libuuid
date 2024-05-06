#ifndef UTIL_LINUX_RANDUTILS
#define UTIL_LINUX_RANDUTILS

#ifdef HAVE_SRANDOM
#define srand(x) srandom(x)
#define rand()	 random()
#endif

void random_get_bytes(void *buf, size_t nbytes);

#endif
