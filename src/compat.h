/* vim: set sw=0 ts=8 cinoptions=:0,t0,+4,(4: */

#ifdef __FreeBSD__
#define __dead __dead2
#endif /* __FreeBSD__ */

#if defined(__linux__) || defined(__CYGWIN__)
#ifdef __GNUC__
#define __dead		__attribute__((__noreturn__))
#else
#define __dead
#endif
#endif /* __linux__ || __CYGWIN__ */
