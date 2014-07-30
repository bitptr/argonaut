AM_CPPFLAGS = -DDATADIR=\"$(datadir)\"
AM_CFLAGS = -Wall -Wextra -pedantic-errors
bin_PROGRAMS = argonaut
dist_argonaut_SOURCES = main.c callbacks.c
argonaut_CFLAGS = $(GTK_CFLAGS) $(AM_CFLAGS)
argonaut_LDFLAGS = $(AM_LDFLAGS) $(GTK_LIBS)
