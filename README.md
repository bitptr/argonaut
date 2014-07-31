Argonaut
========

A spatial directory manager.

Spatial?
--------

Things that make it spatial:

* The same directory appears in the same place on your screen in the same
  dimensions consistently.

  A consequence of this is that directories must open in new windows.

* A window is a directory, and a directory is a window.

  A consequence of this is that each directory can only be open once. When
  combined with virtual desktops the most straightforward way to think about
  the desired functionality is: the existing window closes and a new one opens.

Setup
-----

Depends on GTK+ 3, GModule 2, GIO 2, and BDB.

To develop:

  ./autogen.sh
  ./configure --datadir=$PWD
  make

You may need to set your `CFLAGS` and `LDFLAGS`. These are the recommended
settings for OpenBSD:

    export AUTOMAKE_VERSION=1.14
    export AUTOCONF_VERSION=2.69
    export CFLAGS="-I /usr/local/include/db4"
    export LDFLAGS="-L /usr/local/lib/db4"

Copyright
---------

Copyright 2011, 2014 Mike Burns and Matt Horan
