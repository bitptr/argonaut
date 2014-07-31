AM_CPPFLAGS = -DDATADIR=\"$(datadir)\"
AM_CFLAGS = -Wall -Wextra -pedantic-errors -Wno-unused-parameter -Werror
bin_PROGRAMS = argonaut
dist_argonaut_SOURCES = main.c callbacks.c compat.c directory.c cb_data.c
argonaut_CFLAGS = $(GTK_CFLAGS) $(GIO_CFLAGS) $(GMODULE_CFLAGS) $(BDB_CPPFLAGS) $(AM_CFLAGS)
argonaut_LDFLAGS = $(AM_LDFLAGS) $(GIO_LIBS) $(GMODULE_LIBS) $(BDB_LDFLAGS) $(GTK_LIBS)
