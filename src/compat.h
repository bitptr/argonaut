/* vim: set sw=0 ts=8 cinoptions=:0,t0,+4,(4: */

#ifndef COMPAT_H
#define COMPAT_H

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

#ifndef HAVE_STRLCPY
size_t	strlcpy(char *, const char *, size_t);
#endif /* HAVE_STRLCPY */

#endif /* COMPAT_H */
