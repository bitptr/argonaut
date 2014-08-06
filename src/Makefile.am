AM_CPPFLAGS = -DPKGDATADIR=\"$(pkgdatadir)\"
AM_CFLAGS = -Wall -Wextra -pedantic-errors -Wno-unused-parameter -Werror
bin_PROGRAMS = argonaut
dist_argonaut_SOURCES = main.c main.h callbacks.c compat.c compat.h \
			directory.c directory.h cb_data.c dnd.c dnd.h \
			extern.h pathnames.h
argonaut_CFLAGS = $(GTK_CFLAGS) $(GIO_CFLAGS) $(GMODULE_CFLAGS) $(BDB_CPPFLAGS) $(AM_CFLAGS)
argonaut_LDFLAGS = $(AM_LDFLAGS) $(GIO_LIBS) $(GMODULE_LIBS) $(BDB_LDFLAGS) $(GTK_LIBS)
