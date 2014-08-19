Developing
==========

Principles
----------

- Fewer moving parts.
- We follow the OpenBSD KNF.
- Always able to build and run on the latest OpenBSD and Debian.
- Use the OS and C library first.

Details
-------

*Fewer moving parts*. Fewer libraries, no daemons, no on-disk distractions. The
idea is to reduce the size of the binary and especially to reduce the memory
footprint. The running application should always look simple both in the GUI
and in a ps(1).

*OpenBSD KNF*. See style(9) for details, or [find it online][openbsd-knf]. This
is a way of thinking: explictness in an effort to understand what the software
does, but not too much explictness so that the idea is not muddled by the
syntax. Use these vim settings for the indentation format:

    set sw=0
    set ts=8
    set noet
    set cinoptions=:0,t0,+4,(4

[openbsd-knf]: http://www.openbsd.org/cgi-bin/man.cgi/OpenBSD-current/man9/style.9

*Build and run on OpenBSD and Debian*. These operating systems are the closest
to our values. By supporting them, we also support a wide range of portability.
However, our goal is not portability; our goal is to make this for ourselves
and those like us.

*Use libc first*. It is far too easy to reach for convenience functions in
extra libraries, but we should first use and practice what we have in our OS.
This helps keep the code focused and accessible.

Conventions
-----------

*Name callbacks* after their signal and GtkBuilder id, prefixed with `on_`.

For example, the object id is `window` and the signal is `configure-event`;
therefore the callback function is named `on_window_configure_event`:

	w = GTK_WIDGET(gtk_builder_get_object(builder, "window"));
	g_signal_connect(w, "configure-event",
            G_CALLBACK(on_window_configure_event), NULL);

Setup
-----

See the dependencies in the `README.md`.

To build from a fresh checkout:

    ./autogen.sh
    ./configure
    make pkgdatadir=$PWD bindir=$PWD/src

You may need to set your `CFLAGS` and `LDFLAGS`. These are the recommended
settings for OpenBSD:

    export AUTOMAKE_VERSION=1.14
    export AUTOCONF_VERSION=2.69
    export CFLAGS="-I /usr/local/include/db4"
    export LDFLAGS="-L /usr/local/lib/db4"

The Desktop Entry Specification is installed into `$DATADIR/applications`. If
`$DATADIR` is not one of the system data directories or user data directories,
it will not be picked up by your system menu application. To remedy this, set
`XDG_DATA_HOME`[XDG_DATA_HOME]. For example:

    env XDG_DATA_HOME=$HOME/.localest/share fbpanel &

[XDG_DATA_HOME]: http://standards.freedesktop.org/basedir-spec/latest/ar01s03.html
