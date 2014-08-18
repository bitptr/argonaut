# EXTRA_DIST = org.gnome.argonaut.desktop	org.gnome.argonaut.desktop.in \
# 	     $(desktop_in_files)
# 
# CLEANFILES = $(desktop_DATA) $(NULL)

desktopdir = $(datadir)/applications
dist_desktop_DATA = org.bitptr.argonaut.desktop

edit = sed -e 's|@bindir[@]|$(bindir)|g'

org.bitptr.argonaut.desktop: Makefile
	rm -f $@ $@.tmp
	srcdir=''; \
	  test -f ./$@.in || srcdir=$(srcdir)/; \
	  $(edit) $${srcdir}$@.in >$@.tmp
	chmod +x $@.tmp
	chmod a-w $@.tmp
	mv $@.tmp $@

org.bitptr.argonaut.desktop: $(srcdir)/org.bitptr.argonaut.desktop.in

UPDATE_DESKTOP = update-desktop-database $(datadir)/applications || :

install-data-hook: 
	$(UPDATE_DESKTOP)

uninstall-hook: 
	$(UPDATE_DESKTOP)
