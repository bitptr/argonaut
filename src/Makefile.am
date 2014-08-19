AM_CPPFLAGS = -DBINDIR=\"$(bindir)\" -DPKGDATADIR=\"$(pkgdatadir)\" -DWNCK_I_KNOW_THIS_IS_UNSTABLE
AM_CFLAGS = -Wall -Wextra -pedantic-errors -Wno-unused-parameter -Werror
bin_PROGRAMS = argonaut argonaut-desktop

dist_argonaut_SOURCES = main.c main.h callbacks.c callbacks.h \
			compat.c compat.h directory.c directory.h \
			dnd.c dnd.h window.c window.h state.c state.h \
			file.c file.h store.c store.h thumbnail.c thumbnail.h \
			extern.h pathnames.h
argonaut_CFLAGS = $(GTK_CFLAGS) $(GIO_CFLAGS) $(BDB_CPPFLAGS) $(WNCK_CFLAGS) $(AM_CFLAGS)
argonaut_LDFLAGS = $(AM_LDFLAGS) $(GIO_LIBS) $(BDB_LDFLAGS) $(WNCK_LIBS) $(GTK_LIBS)

dist_argonaut_desktop_SOURCES = desktop.c desktop.h \
																callbacks.c dnd.c state.c store.c \
																callbacks.h dnd.h state.h store.h \
																thumbnail.c thumbnail.h file.c file.h \
																directory.c directory.h window.c window.h \
																extern.h pathnames.h compat.c compat.h
argonaut_desktop_CFLAGS = -Wno-variadic-macros $(GTK_CFLAGS) $(GIO_CFLAGS) $(BDB_CPPFLAGS) $(WNCK_CFLAGS) $(AM_CFLAGS)
argonaut_desktop_LDFLAGS = $(AM_LDFLAGS) $(GIO_LIBS) $(BDB_LDFLAGS) $(WNCK_LIBS) $(GTK_LIBS)
