/* nt_extra.h some extra def which is not in NT but in UNIX. */
#ifndef _nt_extra_h__
#define _nt_extra_h__

#if defined(__cplusplus)
extern "C"
{
#endif

void bzero(void *s, size_t n);
int strcasecmp(const char *s1, const char *s2);
int strncasecmp(const char *s1, const char *s2, int n);
void srandom(unsigned int seed);
int random(void);
srb_long_t strtoll(const char *nptr, char **endptr, int base);


#if defined(__cplusplus)
}
#endif


#endif  /* _nt_extra_h__ */
