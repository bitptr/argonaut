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

Dependencies
------------

Depends on GTK+ 3, GModule 2, GIO 2, and BDB 4+.

On OpenBSD: gtk+3, db

On Debian: libgtk-3-dev, libdb-dev

Copyright
---------

Copyright 2011, 2014 Mike Burns and Matt Horan
